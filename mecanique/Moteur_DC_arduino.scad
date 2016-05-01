$fn=100;

module roue_dentee(){
	difference(){
		cylinder(d=4,h=4);
		cylinder(d=2,h=4);
	}
	for(i=[0:1:9]){
		rotate([0,0,36*i])translate([1.9,-0.5,0])cube([1.01,1,4]);
	}
}

module DC_motor_arduino(){
	difference(){
		union(){
			cube([25.9,15.5,18.3]);
			translate([0,15.5/2,18.3/2])rotate([0,90,0])cylinder(d=24,h=25.9);
			translate([-1.6,15.5/2,18.3/2])rotate([0,90,0])cylinder(d=6.15,h=1.6);
			translate([25.9,15.5/2,18.3/2])rotate([0,90,0])cylinder(d=6.15,h=3);
			translate([-11.5,15.5/2,18.3/2])rotate([0,90,0])cylinder(d=2,h=11.5);
			translate([-7.5,15.5/2,18.3/2])rotate([0,-90,0])roue_dentee();
		}
		translate([0,-0.1,18.3])cube([30.5,15.7,5]);
		translate([0,-0.1,-5])cube([30.5,15.7,5]);
		translate([0,2,18.3/2])rotate([0,90,0])cylinder(d=2.6,h=0.5);
		translate([0,13.5,18.3/2])rotate([0,90,0])cylinder(d=2.6,h=0.5);
		translate([0,15.5/2,3])rotate([0,90,0])cylinder(d=2,h=0.5);

	}
}

DC_motor_arduino();
