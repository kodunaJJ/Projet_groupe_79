$fn=100;

d_ext=30;
d_int=8;
ep_1=8;
l_axe=7;

module poulie_moteur(){
	difference(){
		union(){
			translate([0,0,-3])cylinder(d=d_ext,h=ep_1);
			//translate([0,0,10])cylinder(d=20,h=5);
			translate([0,0,5])cylinder(d=12,h=l_axe);
			translate([0,0,5])cylinder(r1=8,r2=5,h=1.5);

		}
		axe_moteur();
		rotate([-90,90,0])translate([-1.5,0,-5])cylinder(d=2.3,h=20);
		//translate([0,0,5.7])rotate([0,0,0])tore();
	}
}

module axe_moteur(){
	difference(){
		cylinder(d=6.3,h=20);
		translate([-10/2,2.8,0])cube([10,5,20]);
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
