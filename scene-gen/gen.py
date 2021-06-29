import random

def stri(a, b):
    return str(random.randint(a,b))

def rmat():
    return random.choice([
                         #'GENERIC_SOLID',
                         #'GLOWING',
                         #'BLACK_HOLE',
                         'MIRROR',
                         'SHINY',
                         'MATT',
                         'GLASS'])

rng=300

for i in range (rng):
    print('{ .pos={'+stri(-20000,20000)+','+'-1600'+','+stri(-20000,20000)+'},   .r='+str(450)+',    .color={'+stri(0,255)+','+stri(0,255)+','+stri(0,255)+'},   .material='+rmat()+' },')

#for i in range(rng):
#    print('{.obj_ptr=&spheres['+str(i)+'], .obj_type=SPHERE},')
