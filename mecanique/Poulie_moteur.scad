$fn=100;

d_ext=20;
d_int=8;
ep_1=5;
l_axe=5;

module poulie_moteur(){
	difference(){
		union(){
			cylinder(d=d_ext,h=ep_1);
			//translate([0,0,10])cylinder(d=20,h=5);
			translate([0,0,5])cylinder(d=10,h=l_axe);
			translate([0,0,5])cylinder(r1=6,r2=5,h=0.5);
		}
		axe_moteur();
		translate([0,0,5.7])rotate([0,0,0])#tore();
	}
}

module axe_moteur(){
	difference(){
		cylinder(d=6.3,h=20);
		translate([-10/2,2.75,0])cube([10,5,20]);
	}
}

module tore(){
	difference() {
		rotate_extrude () translate([5.7,0,0]) circle (d=1.4, $fn=100);
		//translate([-23/2,-23/2,0])cube([23,23,3]);
	}
}

poulie_moteur();
//axe_moteur();
//tore();
