import sys
import pandas as pd

df = pd.read_csv(sys.argv[1])

q1 = df.iloc[:, 1].quantile(0.0)
q3 = df.iloc[:, 1].quantile(0.95)

df = df[df.iloc[:, 1].between(q1, q3)]

df.to_csv(sys.argv[2], index=False);