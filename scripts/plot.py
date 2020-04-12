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



def plot_predictions():
    ground_truth = [121, 200, 234, 291, 428, 621, 904, 1128, 1546, 1891, 2201, 2433, 2915, 3417, 3904, 4256, 4579, 5717, 6836, 7910, 9056, 10278, 11130, 12056, 13717, 15927, 17857, 19638, 20727];
    #predicted = [121, 200, 234, 291, 428, 621, 904, 1128, 1546, 1891, 2201, 2433, 2915, 3417, 3904, 4256, 4579, 5717, 6836, 7910, 9056, 10278, 11130, 12056, 13717, 15328, 17029, 18941, 21023, 23413]
    x_pred = [25, 26, 27, 28, 29]
    predicted = [15328, 17029, 18941, 21023, 23413]


    y_min = np.array(predicted) * 0.9
    y_max = np.array(predicted) * 1.1

    fig, ax = plt.subplots()
    ax.plot(ground_truth, 'o-', label='Ground truth')
    ax.plot(x_pred, predicted, 'x-', label='Predicted')
    ax.fill_between(x_pred, y_min, y_max, alpha=0.2)
    ax.set(xlabel='Days after 100th notification', title='Covid19 - Predicted cases')
    ax.grid()
    ax.legend()

    plt.show()
    


if __name__ == "__main__":

    # plot_2_codes("cases", "USA", "CHN")
    plot_predictions()
