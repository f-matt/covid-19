#!/usr/bin/python
# -*- coding:utf-8 -*-
import sys
import psycopg2
import datetime
import csv

LABELS = [
    "cases",
    "deaths"
]

FILES = [
    "../reports/covid-confirmed-cases-since-100th-case-202003.csv",
    "../reports/covid-confirmed-deaths-since-5th-death-202003.csv"
]

SELECT_STMT = [
    """select cases
        from notifications
        where code = %s
        and notification_date = %s""",
    """select deaths
        from notifications
        where code = %s
        and notification_date = %s"""
]

UPDATE_STMT = [
    """update notifications
        set cases = %s
        where code = %s
        and notification_date = %s""",
    """update notifications
        set deaths = %s
        where code = %s
        and notification_date = %s"""
]

INSERT_STMT = [
    """insert into notifications(entity, code, notification_date, cases)
        values (%s, %s, %s, %s)""",
    """insert into notifications(entity, code, notification_date, deaths)
        values (%s, %s, %s, %s)"""
]


def import_data(label):

    if label not in LABELS:
        print "Label " + label + " not found"
        return

    idx = LABELS.index(label)

    try:
        inserted = 0
        updated = 0

        fid = open(FILES[idx], "r")
        reader = csv.reader(fid, delimiter=',')

        conn = psycopg2.connect("dbname='covid19' user='postgres' host='localhost' password='123456'")
        cur = conn.cursor()

        while True:
            line = reader.next()
            if line[0] == 'Entity':
                continue

            entity = line[0]
            code = line[1]
            if not code:
                continue

            notification_date = datetime.datetime.strptime(line[2], '%b %d, %Y')

            try:
                count = int(line[3])
            except ValueError:
                count = 0

            cur.execute(SELECT_STMT[idx], (code, notification_date))
            row = cur.fetchone()
            if row:
                if row[0] != count:
                    cur.execute(UPDATE_STMT[idx], (count, code, notification_date))
                    updated += 1
            else:
                cur.execute(INSERT_STMT[idx], (entity, code, notification_date, count))
                inserted += 1
    except StopIteration:
        conn.commit()
        print "Finished import: " + str(inserted) + " records inserted and " + str(updated) + " records updated."
    finally:
        if fid:
            fid.close()
        if cur:
            cur.close()
        if conn:
            conn.close()

if __name__ == "__main__":

    import_data("cases")
