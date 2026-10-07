#!/usr/bin/env python3
"""Check deterministic terrain ratios using the same formula as OriginRuntime.gd."""
import math

W = D = 96
SEED = 7
SEA = 7.2

def hash01(x, z):
    n = SEED * 374761393 + x * 668265263 + z * 2147483647
    n = ((n ^ (n >> 13)) * 1274126177)
    n = n ^ (n >> 16)
    return (n & 2147483647) / 2147483647.0

def value_noise(x, z, scale, ox, oz):
    gx, gz = x / scale, z / scale
    x0, z0 = math.floor(gx), math.floor(gz)
    tx, tz = gx - x0, gz - z0
    tx = tx * tx * (3.0 - 2.0 * tx)
    tz = tz * tz * (3.0 - 2.0 * tz)
    n00 = hash01(int(x0 + ox), int(z0 + oz))
    n10 = hash01(int(x0 + 1 + ox), int(z0 + oz))
    n01 = hash01(int(x0 + ox), int(z0 + 1 + oz))
    n11 = hash01(int(x0 + 1 + ox), int(z0 + 1 + oz))
    nx0 = n00 * (1.0 - tx) + n10 * tx
    nx1 = n01 * (1.0 - tx) + n11 * tx
    return nx0 * (1.0 - tz) + nx1 * tz

heights=[]
for z in range(D):
    for x in range(W):
        continent=(value_noise(x,z,48.0,17,31)-0.5)*8.0
        regional=(value_noise(x,z,20.0,73,-19)-0.5)*4.5
        ridge=value_noise(x,z,11.0,123,55)
        ridges=(1.0-abs(ridge*2.0-1.0))**2*3.5
        peak_factor=max(0.0,(ridge-0.58)/0.42)
        mountain_peaks=peak_factor**2.2*5.0
        waves=math.sin(x*0.065+z*0.035)*0.55+math.cos(x*0.032-z*0.071)*0.45
        heights.append(max(2.0,min(22.0,5.7+continent+regional+ridges+mountain_peaks+waves)))
water=sum(h<SEA for h in heights)/len(heights)
assert min(heights) < SEA < max(heights), 'terrain must contain both water and land'
assert 0.15 < water < 0.50, f'water ratio out of expected range: {water:.3f}'
assert max(heights)-min(heights) > 7.0, 'terrain relief is too flat'
print('ORIGIN TERRAIN DISTRIBUTION: PASS')
print('height_min=%.3f height_max=%.3f water_ratio=%.3f land_ratio=%.3f' % (min(heights),max(heights),water,1.0-water))
