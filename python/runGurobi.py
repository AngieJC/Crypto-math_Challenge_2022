from gurobipy import *
import math
'''
for case in range(10):
    print('case '+str(case))
    m = read('AES_Model3_Simplified_5r_collide3_case{}.lp'.format(case))
    m.setParam( 'OutputFlag', 1 )
    #m.Params.timeLimit = 1800.0
    #m.Params.BestObjStop = 12.5
    m.optimize()
    
    #m.write("AES_Model3_simplified\AES_Model3_Simplified_3r_collide3_case4.sol")


'''
m=read('Jdiff_5r.lp')
m.optimize()
m.write("Jdiff_5r.sol")



