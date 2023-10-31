import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import norm

rtp = []
s = _s = n = 0

for i in range(5):
    fileName = "rtpLogs/rtpLog" + str(i + 1) + ".txt"
    print(fileName)
    with open(fileName) as f:
        raw_content = f.read()
        content = raw_content.split()
        flag = 0
        for x in content:
            if(flag):
                y = float(x)
                rtp.append(y)
                n += 1
                if(y > 1):
                    s += (y - 1) * (y - 1)
                    _s += (y - 1)
            flag ^= 1

mu, std = norm.fit(rtp)
# Draw the plot
plt.hist(rtp, bins = 1000, color = 'blue', edgecolor = 'blue', density = True)

xmin, xmax = plt.xlim()
x = np.linspace(xmin, xmax, 100)
p = norm.pdf(x, mu, std)
plt.plot(x, p, 'k', linewidth=2)
title = "Fit results: mu = %.2f,  std = %.2f" % (mu, std)
print(mu, std)
print(n, _s / n, s / n)

plt.show()