"""
Author: Matej Otcenas(xotcen01)
Date: 1.5.2022
Description: Module for plotting the statistics.
"""

import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt 

"""
Plots for the evolution.
Mainly consising of scatter plots, pareto line and boxplots
"""
def create_statistics(name, save_path):
    df = pd.read_csv(save_path + name + ".csv")
    sns.scatterplot(data = df, x = "params", y = "acc", hue='id', legend = False)
    plt.savefig(save_path + name + "params_acc.pdf", dpi=400)
    plt.show()

    sns.scatterplot(data = df, x = "gen", y = "acc", hue='id', legend = False)
    plt.savefig(save_path + name + "gen_acc.pdf", dpi=400)
    plt.show()

    sns.scatterplot(data = df, x = "gen", y = "params", hue='id', legend = False)
    plt.savefig(save_path + name + "gen_params.pdf", dpi=400)
    plt.show()

    # boxplots and pareto line only for the all individuals generated in evolution
    if name == "all_ind":
        plot_pareto(df['params'], df['acc'], save_path)

        sns.boxplot(x=df["gen"], y=df["acc"])
        plt.savefig(save_path + "boxplot_acc.pdf", dpi=400)
        plt.show()

        sns.boxplot(x=df["gen"], y=df["params"])
        plt.savefig(save_path + "boxplot_param.pdf", dpi=400) 
        plt.show()



'''
Method to take two equally-sized lists and return just the elements which lie 
on the Pareto frontier, sorted into order.
Default behaviour is to find the maximum for both X and Y, but the option is
available to specify maxX = False or maxY = False to find the minimum for either
or both of the parameters.
'''
# https://oco-carbon.com/metrics/find-pareto-frontiers-in-python/
def pareto_frontier(Xs, Ys, maxX = True, maxY = True):

    myList = sorted([[Xs[i], Ys[i]] for i in range(len(Xs))], reverse=maxX)
    p_front = [myList[0]]    
    for pair in myList[1:]:
        if maxY: 
            if pair[1] >= p_front[-1][1]: # Look for higher values of Y…
                p_front.append(pair) # … and add them to the Pareto frontier
        else:
            if pair[1] <= p_front[-1][1]: # Look for lower values of Y…
                p_front.append(pair) # … and add them to the Pareto frontier

    p_frontX = [pair[0] for pair in p_front]
    p_frontY = [pair[1] for pair in p_front]
    return p_frontX, p_frontY

def plot_pareto(Xs, Ys, save_path):
    # Find lowest values for params and highest for acc
    p_front = pareto_frontier(Xs, Ys, maxX = False, maxY = True) 
    # Plot a scatter graph of all results
    plt.scatter(Xs, Ys)
    # Then plot the Pareto frontier on top
    plt.plot(p_front[0], p_front[1])
    plt.savefig(save_path + "paretoline.pdf", dpi=400) 
    plt.show()
