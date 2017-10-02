innerDiameter = 2*50/PI;
outerDiameter = innerDiameter+6;

outerHeight = 11;
innerHeight = 8;
stepperShaftDiameter = 5.2; // default is 5, due to over-extrusion we had to change this; act accordingly 

//main pulley body
difference(){
    cylinder(d=outerDiameter, h=outerHeight, $fn=60);
    cylinder(d=stepperShaftDiameter, h=outerHeight, $fn=60);
    
    translate([0, 0, outerHeight/2-innerHeight/2]){
            
        difference(){
            translate([-outerDiameter/2, -outerDiameter/2, 0]) cube([outerDiameter, outerDiameter, innerHeight]);
            
            cylinder(r2=innerDiameter/2, r1=outerDiameter/2, h=1, $fn=100);
            translate([0, 0, 1]) cylinder(r=innerDiameter/2, h=innerHeight-2, $fn=100);
            translate([0, 0, innerHeight-1]) cylinder(r1=innerDiameter/2, r2=outerDiameter/2, h=1, $fn=100);
        }
    }
}

// locking screw and nut
translate([0, 0, outerHeight])
{
    difference()
    {
        cylinder(d=25, h=10, $fn=60);
        cylinder(d=stepperShaftDiameter, h=10, $fn=60);
        
        translate([0, 0, 5]) rotate([90, 0, 0]) cylinder(d=4, h=15, $fn=40);
        translate([-2.6, -6, 2.5]) cube([5.2, 2, 10]); 
    }
}