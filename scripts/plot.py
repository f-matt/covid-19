#!/usr/bin/python
# -*- coding:utf-8 -*-
import matplotlib
import matplotlib.pyplot as plt
import numpy as np

import psycopg2


def get_cases_after_100(code):
    try:
        conn = psycopg2.connect("dbname='covid19' user='postgres' host='localhost' password='123456'")
        cur = conn.cursor()

        cur.execute("""select cases
                        from notifications
                        where code = %s
                        and cases > 100
                        order by notification_date""", (code,))
        rows = cur.fetchall()

        cases = list()

        for i in range(len(rows)):
            cases.append(rows[i][0])

        return cases
    except Exception as e:
        print "Exception reached: " , e
        return None
    finally:
        if cur:
            cur.close()
        if conn:
            conn.close()


def get_deaths_after_5(code):
    try:
        conn = psycopg2.connect("dbname='covid19' user='postgres' host='localhost' password='123456'")
        cur = conn.cursor()

        cur.execute("""select deaths
                        from notifications
                        where code = %s
                        and deaths > 5
                        order by notification_date""", (code,))
        rows = cur.fetchall()

        deaths = list()

        for i in range(len(rows)):
            deaths.append(rows[i][0])

        return deaths
    except Exception as e:
        print "Exception reached: " , e
        return None
    finally:
        if cur:
            cur.close()
        if conn:
            conn.close()



def plot_cases():

    try:
        conn = psycopg2.connect("dbname='covid19' user='postgres' host='localhost' password='123456'")
        cur = conn.cursor()

        cur.execute("""select notification_date, cases, deaths
                        from notifications
                        where code = %s
                        and cases > 100
                        order by notification_date""", ('BRA',))
        rows = cur.fetchall()

        day0 = rows[0][0]

        x = list()
        cases = list()
        deaths = list()

        for i in range(len(rows)):
            x.append(i)
            cases.append(rows[i][1])
            deaths.append(rows[i][2])

        fig, ax = plt.subplots()
        ax.plot(x, cases, 'o-')
        ax.plot(x, deaths, 'x-')
        ax.set(xlabel='Days', ylabel='Cases',
               title='Reference date: ' + day0.strftime("%d-%b-%Y"))
        ax.grid()

        plt.show()
    except Exception as e:
        print "Exception reached: " , e
    finally:
        if cur:
            cur.close()
        if conn:
            conn.close()


def plot_2_codes(label, code1, code2):

    if label ==  'cases':
        y1 = get_cases_after_100(code1)
        y2 = get_cases_after_100(code2)
    else:
        y1 = get_deaths_after_5(code1)
        y2 = get_deaths_after_5(code2)

    fig, ax = plt.subplots()
    ax.plot(y1, 'o-', label=code1)
    ax.plot(y2, 'o-', label=code2)
    ax.set(xlabel='Days', title=label)
    ax.grid()
    ax.legend()

    plt.show()



if __name__ == "__main__":

    plot_2_codes("cases", "USA", "CHN")
