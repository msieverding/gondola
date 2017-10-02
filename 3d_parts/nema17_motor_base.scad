motorSide = 42.2;
motorHeight = 46.3;
motorMountingHoles = 31;
motorHolesLength = 4.5;

baseSide = 70;
baseHeight = 3;

wallWidth = 4;

// mounting base plate
difference()
{
    cube([baseSide, baseSide, baseHeight]);
    
    translate([5, 5, 0]) cylinder(d=5, h=baseHeight, $fn=20);
    translate([baseSide-5, 5, 0]) cylinder(d=5, h=baseHeight, $fn=20);
    translate([5, baseSide-5, 0]) cylinder(d=5, h=baseHeight, $fn=20);
    translate([baseSide-5, baseSide-5, 0]) cylinder(d=5, h=baseHeight, $fn=20);
    
}
// main enclosure
translate([(baseSide-motorSide)/2-wallWidth, (baseSide-motorSide)/2-wallWidth, baseHeight])
{
    difference()
    {
        union()
        {
            cube([motorSide+wallWidth*2, motorSide+wallWidth*2, motorHeight]);
            translate([0, 0, motorHeight])
            {
               difference()
               {
                   // thread hole and support
                   cube([20, 20, 12]);
                   translate([9, 0, 9]) rotate([-90, 0, 0]) cylinder(d=2.5, h=10, $fn=10);
               }
           }
       }
   
        
        translate([wallWidth+sqrt(2)*2.5, wallWidth, 0])
            minkowski()
            {
                cube([motorSide-sqrt(2)*5, motorSide-sqrt(2)*5, motorHeight]);
                rotate([0, 0, 45]) cube([5, 5, motorHeight]);
            }
         
        // latches   
        translate([motorSide/2+wallWidth-3.5, motorSide+wallWidth, motorHeight-20])        
            cube([0.5, wallWidth, 20]);
        translate([motorSide/2+wallWidth+3, motorSide+wallWidth, motorHeight-20])
            cube([0.5, wallWidth, 20]);
            
        translate([0, motorSide/2+wallWidth-3.5, motorHeight-20])
            cube([wallWidth, 0.5, 20]);
        translate([0, motorSide/2+wallWidth+3, motorHeight-20])
            cube([wallWidth, 0.5, 20]);
            
        translate([motorSide+wallWidth, motorSide/2+wallWidth-3.5, motorHeight-20])
            cube([wallWidth, 0.5, 20]);
        translate([motorSide+wallWidth, motorSide/2+wallWidth+3, motorHeight-20])
            cube([wallWidth, 0.5, 20]);
       
        // wire hole
        translate([motorSide/2+wallWidth-5, 0, 0])
            cube([10, wallWidth, 10]);
            
        // wire shaft
        translate([motorSide/2+wallWidth-5, wallWidth-2.5, 0])
            cube([10, 2.5, motorHeight]);
    }
    
    // latches top
    translate([motorSide/2+wallWidth-3, motorSide+wallWidth-1, motorHeight])
        cube([6, wallWidth+1, 2]);
    translate([0, motorSide/2+wallWidth-3, motorHeight])
        cube([wallWidth+1, 6, 2]);    
    translate([motorSide+wallWidth-1, motorSide/2+wallWidth-3, motorHeight])
        cube([wallWidth+1, 6, 2]);    
  
}