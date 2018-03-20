
def number_of_trees(extraCO2,i):

    co2 = 2.72  * extraCO2
    value = (400 +2*i) * 2.72
    
    if  co2 > value:
        co2left = 2.72 * (extraCO2 - 400- 2*i)
        if  0 <= i <= 10:
           Nooftrees = ((co2left)/ (i/2(2*13+ (i-1)*3.5))) +1
           print "Number of trees: ", int (Nooftrees);
        else:
            Nooftrees = (co2left)/(287.5 + (i-10)*48)+1;
            print "Number of trees: ",  int (Nooftrees);
           
    else: 
       return 0 
       
number_of_trees(500,12)


