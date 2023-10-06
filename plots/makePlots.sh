#!/bin/bash

mkdir ../out/plots

python3 merge_bar.py
python3 mixedMerge_bar.py
python3 boruvkaMerge_bar.py
python3 boruvka_bar.py

python3 weak-scale_plot.py

python3 merge_boxplot.py
python3 mixedMerge_boxplot.py
python3 boruvka_boxplot.py
python3 boruvkaMerge_boxplot.py

#python3 boruvka_message_overlap.py
