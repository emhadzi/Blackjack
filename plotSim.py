import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import norm

rtp = []

for i in range(5):
    fileName = "rtpLog" + str(i + 1) + ".txt"
    print(fileName)
    with open(fileName) as f:
        raw_content = f.read()
        content = raw_content.split()
        flag = 0
        for x in content:
            if(flag):
                rtp.append(float(x))
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

plt.show()