"""Generate plots only from measured MPI output; no invented timing data."""
from pathlib import Path
import csv, statistics
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

folder=Path(__file__).resolve().parent
with (folder/'timings.csv').open() as stream:
    rows=list(csv.DictReader(stream))
groups={}
for row in rows:
    name=row['program']; processes=int(row['processes']); elapsed=float(row['seconds'])
    assert elapsed>0
    if name=='sum': assert int(row['result'])==50000005000000
    else:
        assert abs(float(row['pi'])-3.141592653589793)<0.01
    groups.setdefault((name,processes),[]).append(elapsed)
counts={int(row['result']) for row in rows if row['program']!='sum'}
assert len(counts)==1, 'Pi variants/process counts disagree'
medians={key:statistics.median(values) for key,values in groups.items()}
with (folder/'summary.csv').open('w',newline='') as stream:
    writer=csv.writer(stream);writer.writerow(['program','processes','median_seconds','speedup'])
    for (name,p),seconds in sorted(medians.items()):
        writer.writerow([name,p,seconds,medians[(name,1)]/seconds])
for metric in ['time','speedup']:
    fig,ax=plt.subplots(figsize=(8,5))
    for name in sorted({key[0] for key in medians}):
        ps=sorted(p for n,p in medians if n==name)
        values=[medians[(name,p)] if metric=='time' else medians[(name,1)]/medians[(name,p)] for p in ps]
        ax.plot(ps,values,'o-',label=name)
    ax.set(xlabel='MPI processes (single EC2 node, 1 available CPU)',ylabel='Median elapsed time (seconds)' if metric=='time' else 'Speedup T(1) / T(p)', title='Lab 8: measured MPI '+metric+' (oversubscribed)')
    ax.grid(alpha=.3);ax.legend();fig.tight_layout();fig.savefig(folder/(metric+'.png'),dpi=180);plt.close(fig)
print('Validated numerical results and created time.png, speedup.png, summary.csv')
