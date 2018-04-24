const Net=require('net');
const EventEmitter=require('events').EventEmitter;
const Rosrun=require('./rosrun.js');
const Notifier=new EventEmitter;

const ros=require('rosnodejs');
const sensor_msgs=ros.require('sensor_msgs').msg;
const std_msgs=ros.require('std_msgs').msg;
const std_srvs=ros.require('std_srvs').srv;
const dyn_msgs=ros.require('dynamic_reconfigure').msg;
const dyn_srvs=ros.require('dynamic_reconfigure').srv;
const gev_srvs=ros.require('camera_aravis').srv;
const rovi_srvs=ros.require('rovi').srv;

let run_c;  //should be rosrun.js camnode
let rosNode;

const reg_table={
	'AcquisitionStart': 0x00100018,
	'AcquisitionStop': 0x0010001c,
	'AcquisitionFrameRate': 0x00101000,
	'ExposureTime': 0x00101004,
	'GainAnalog': 0x00101008,
	'Gain': 0x0010100C,
	'TriggerMode': 0x00101010,
	'SerialPort': 0x00300004
}
const val_table={
	'Off': 0,
	'On': 1
}

var ycam={
	cset:async function(obj){
		let greq=new gev_srvs.GevRegs.Request();
		let dreq=new dyn_srvs.Reconfigure.Request();
		for(let key in obj){
			let val=obj[key];
			if(reg_table.hasOwnProperty(key)){
				greq.address=reg_table[key];
				greq.data= typeof(val)=='string'? val_table[val]:val;
				let res=await run_c.reg_write.call(greq);
			}
			else if(typeof(val)=='string'){
				let param=new dyn_msgs.StrParameter();
				param.name=key;
				param.value=val;
				dreq.config.strs.push(param);
			}
			else{
				let param=new dyn_msgs.DoubleParameter();
				param.name=key;
				param.value=val;
				dreq.config.doubles.push(param);
			}
		}
		if(dreq.config.strs.length>0 || dreq.config.doubles.length>0){
			let res=await run_c.dynparam_set.call(dreq);
		}
		return true;
	},
	pregbuf:'',
	pregwrt:async function(){
		if(ycam.pregbuf.length>0){
			let greq=new gev_srvs.GevRegs.Request();
			greq.address=reg_table['SerialPort'];
			let lsb=ycam.pregbuf.charCodeAt(0);
			greq.data=(~lsb<<16)|lsb;
			let res=await run_c.reg_write.call(greq);
//			setTimeout(function(){
			ycam.pregbuf=ycam.pregbuf.slice(1);
			ycam.pregwrt();
//			},1000);
		}
	},
	pset:function(str){
		let s0=this.pregbuf;
		this.pregbuf=s0+str+'\n';
		if(s0.length==0) this.pregwrt();
	},
	normal:false,
	stat:function(){
		return {'cam':run_c.running};
	},
	scan:function(){
		let s;
		try{
			s=this.stat();
		}
		catch(err){
			Notifier.emit('stat',this.normal=false);
			setTimeout(function(){ ycam.scan();},1000);
			return;
		}
		let f=true;
		for(let key in s) f=f && s[key];
		if(f==undefined) f=false;
		Notifier.emit('stat',this.normal=f);
		setTimeout(function(){ ycam.scan();},1000);
	},
	open:function(nh,ns){
		rosNode=nh;
		run_c=Rosrun.run('camera_aravis camnode',ns);
		run_c.on('start',async function(){
			if(! await openCamera(run_c,ns)){
				ros.log.error('Failure in openCamera');
				process.exit(101);
			}
			Notifier.emit('wake');
		});
		this.scan();
		return Notifier;
	}
}

function openCamera(rosrun,ns){
	let image_l=new sensor_msgs.Image();
	let image_r=new sensor_msgs.Image();
	image_l.data=new Uint8Array();
	image_r.data=new Uint8Array();
	let sub=rosNode.subscribe(ns+'/camera/image_raw',sensor_msgs.Image,(src)=>{
		const h=image_l.height=image_r.height=src.height;
		const w=image_l.step=image_r.step=src.step/2;
		image_l.header=image_r.header=src.header;
		image_l.width=image_r.width=src.width/2;
		image_l.encoding=image_r.encoding=src.encoding;
		let sz=image_l.step*image_l.height;
		if(sz>image_l.data.byteLength) image_l.data=new Uint8Array(sz);
		if(sz>image_r.data.byteLength) image_r.data=new Uint8Array(sz);
		for(let i=0,s=0,t=0;i<h;i++,t+=w){
			image_l.data.subarray(t).set(src.data.subarray(s,s+w));
			s+=w;
			image_r.data.subarray(t).set(src.data.subarray(s,s+w));
			s+=w;
		}
		Notifier.emit('cam_l',image_l);
		Notifier.emit('cam_r',image_r);
	});
	return new Promise(async function(resolve){
		let regw=rosNode.serviceClient(ns+'/camera/regw',gev_srvs.GevRegs,{persist:true});
		if(await rosNode.waitForService(regw.getService(),2000)) rosrun.reg_write=regw;
		else{
			ros.log.error(ns+'/"regw" not available');
			resolve(false);
		}
		let cset=rosNode.serviceClient(ns+'/camera/set_parameters',dyn_srvs.Reconfigure,{persist:true});
		if(await rosNode.waitForService(cset.getService(),2000)) rosrun.dynparam_set=cset;
		else{
			ros.log.error(ns+'/"set_parameters" not available');
			resolve(false);
		}
		resolve(true);
	});
}

module.exports=ycam;