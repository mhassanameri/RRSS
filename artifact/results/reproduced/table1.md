# Reproduced Table 1: fPAKE Performance

This table is generated from the benchmark `statistics.txt` files. Timing values are in seconds. Communication overhead values are in KiB.

| Algorithm | Sec. | Type | Role | Run time (calc) | Comm. time (net) | Overall time | Role comm. ovhd. (KiB) | Total comm. ovhd. (KiB) |
|---|---|---|---|---|---|---|---|---|
| fPAKE RSS | 128 | acc_h-bar | Sender | 0.0162 | 0.0098 | 0.0260 | 3.3008 | 5.1289 |
| fPAKE RSS | 128 | acc_h-bar | Receiver | 0.0524 | 0.0162 | 0.0686 | 1.8281 | 5.1289 |
| fPAKE RSS | 128 | acc_h-gyrW | Sender | 0.0144 | 0.0087 | 0.0230 | 3.6523 | 5.6836 |
| fPAKE RSS | 128 | acc_h-gyrW | Receiver | 0.0634 | 0.0143 | 0.0777 | 2.0312 | 5.6836 |
| fPAKE RRSS | 128 | acc_h-bar | Sender | 0.0948 | 0.0064 | 0.1012 | 34.2031 | 36.0312 |
| fPAKE RRSS | 128 | acc_h-bar | Receiver | 0.0306 | 0.0948 | 0.1254 | 1.8281 | 36.0312 |
| fPAKE RRSS | 128 | acc_h-gyrW | Sender | 0.1289 | 0.0072 | 0.1361 | 41.7383 | 43.7695 |
| fPAKE RRSS | 128 | acc_h-gyrW | Receiver | 0.0399 | 0.1289 | 0.1688 | 2.0312 | 43.7695 |

`Total comm. ovhd.` is the sum of Sender and Receiver communication overhead for the same algorithm, security level, and input type.
