#!/usr/bin/python
# -*- coding:utf-8 -*-
import psycopg2
import numpy as np


def create_dataset(experiment_id, input_size):
    try:
        conn = psycopg2.connect("dbname='covid19' user='postgres' host='localhost' password='123456'")
        cur = conn.cursor()

        # Select codes with enough samples
        cur.execute("""select code
            from notifications
            where cases > 100
            and code <> 'OWID_WRL'
            group by code
            having count(*) > %s""", (input_size,))
        codes = cur.fetchall()

        sample_id = 1
        samples = list()
        sample_size = input_size + 1

        for code in codes:
            cases = list()
            cur.execute("""select cases
                from notifications
                where code = %s
                and cases > 100
                order by notification_date""", (code[0],))
            rset = cur.fetchall()

            for r in rset:
                cases.append(r[0])

            for i in range(len(cases) - sample_size + 1):
                slice = cases[i:i+sample_size]
                samples.append(slice)

        np.random.shuffle(samples)

        n_val_samples = int(round(0.1 * len(samples)))

        db_samples = list()

        for i in range(len(samples)):
            # First n_val_samples used for test
            if i < n_val_samples:
                type = "X"
            elif i < (2 * n_val_samples):
                type = "V"
            else:
                type = "T"

            # Target
            cur.execute("""insert into samples(experiment_id, sample_id, seq, value, type)
                values (%s, %s, %s, %s, %s)""",(experiment_id,
                    i + 1,
                    1,
                    samples[i][-1],
                    type));

            seq = 2
            for j in range(input_size):
                cur.execute("""insert into samples(experiment_id, sample_id, seq, value, type)
                    values (%s, %s, %s, %s, %s)""",(experiment_id,
                        i + 1,
                        seq + j,
                        samples[i][j],
                        type));

        conn.commit()

    except Exception as e:
        print "Exception reached: " , e
        return None
    finally:
        if cur:
            cur.close()
        if conn:
            conn.close()


if __name__ == "__main__":
    create_dataset(experiment_id=2, input_size=4)
