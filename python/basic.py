# 操作&作为2进1出的S盒的差分分布表
def andbox(x):
    return (x&(x>>1))&0x1

diff_tab=[[0,0] for i in range(2**2)]
for indiff in range(2**2):
    for x in range(2**2):
        outdiff = andbox(x)^andbox(x^indiff)
        diff_tab[indiff][outdiff] += 1
print(diff_tab)

r=[]
for i in range(8):
    a=i&0x1
    b=(i>>1)&0x1
    c=(i>>2)&0x1
    r.append(tuple([a,b,c,a^b^c]))
print(r)
