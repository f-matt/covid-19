#!/usr/bin/python
# -*- coding:utf-8 -*-
import sys
import psycopg2
import datetime
import csv

CASES_FILE = "../reports/covid-confirmed-cases-since-100th-case.csv"
DEATHS_FILE = "../reports/covid-confirmed-deaths-since-5th-death.csv"

def import_cases_data():

    try:
        inserted = 0
        updated = 0

        fid = open(CASES_FILE, "r")
        reader = csv.reader(fid, delimiter=',')

        conn = psycopg2.connect("dbname='covid19' user='postgres' host='localhost' password='123456'")
        cur = conn.cursor()

        while True:
            line = reader.next()

            if line[0] == 'Entity':
                continue

            entity = line[0]
            code = line[1]
            notification_date = datetime.datetime.strptime(line[2], '%b %d, %Y')

            try:
                cases = int(line[3])
            except ValueError:
                cases = 0

            cur.execute("""select cases
                from notifications
                where code = %s
                and notification_date = %s""", (code, notification_date))

            row = cur.fetchone()

            if row:
                if row[0] != int(line[3]):
                    cur.execute("""update notifications
                        set cases = %s
                        where code = %s
                        and notification_date = %s""", (cases, code, notification_date))

                    updated += 1
            else:
                cur.execute("""insert into notifications(entity, code, notification_date, cases)
                    values (%s, %s, %s, %s)""",
                    (entity, code, notification_date, cases))

                inserted += 1

    except StopIteration:
        conn.commit()
        print "Finished import: " + str(inserted) + " records inserted and " + str(updated) + " records updated."
    except:
        print "Reached exception: "
        print sys.exc_info()
        return

    finally:
        if fid:
            fid.close()

        if cur:
            cur.close()

        if conn:
            conn.close()



if __name__ == "__main__":

    import_cases_data()
