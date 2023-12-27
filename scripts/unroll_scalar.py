
# /*θ*/ FOR(x,5) C[x]=rL(x,0)^rL(x,1)^rL(x,2)^rL(x,3)^rL(x,4); FOR(x,5) { D=C[(x+4)%5]^ROL(C[(x+1)%5],1); FOR(y,5) XL(x,y,D); }
for x in range(5):
    print(f"u64 C_{x}= rL({x},0) ^ rL({x},1) ^ rL({x},2) ^ rL({x},3) ^ rL({x},4);")
for x in range(5):
    print(f"u64 D_{x} = C_{(x+4)%5} ^ ROL(C_{(x+1)%5},1);")
    for y in range(5):
        print(f"XL({x},{y},D_{x});")

# /*ρπ*/ x=1; y=r=0; D=rL(x,y); FOR(j,24) { r+=j+1; Y=(2*x+3*y)%5; x=y; y=Y; C[0]=rL(x,y); wL(x,y,ROL(D,r%64)); D=C[0]; }
x = 1; y = r = 0
print(f"u64 T_0 = rL({x}, {y});")
for j in range(24): 
    r += j+1
    Y = (2*x+3*y)%5
    x=y
    y=Y
    print(f"u64 T_{j+1} = rL({x}, {y});")
    print(f"wL({x}, {y}, ROL(T_{j}, {r%64}));")
# /*χ*/ FOR(y,5) { FOR(x,5) C[x]=rL(x,y); FOR(x,5) wL(x,y,C[x]^((~C[(x+1)%5])&C[(x+2)%5])); }
for y in range(5):
    for x in range(5):
        print(f"u64 C_{y}_{x} = rL({x}, {y});")
    for x in range(5):
        print(f"wL({x}, {y}, C_{y}_{x} ^ (~C_{y}_{(x+1) % 5} & C_{y}_{(x+2) % 5}));")
