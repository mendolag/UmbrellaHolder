/**
 * Created by josmendola on 14/04/16.
 */

var User =require('./models/User');

var Blue=require('./models/Bluetooth');
var Reminder=require('./models/Reminder');
var mongoose=require('mongoose');
var ObjectId = mongoose.Types.ObjectId;

/* ********************************************************** */
/* ********************* USER FUNCTIONS ********************* */
/* ********************************************************** */
exports.getUser=function(username,callback) {
    User.findOne({'username':username},function(err,user){
        if(err){
            callback(err,null);
        }else if(user){

            callback(null,user);
        }else{
            callback(null,null);
        }
    });
};

exports.getUserByBlue=function(bleName,callback){
    Blue.findOne({bleName:bleName},function(err,ble){
        if(err){callback(err,null)}
        else if(ble){
            User.findOne({bluetoothId:ObjectId(ble._id)},function (err ,user) {
                console.log("User found:" +user);
                if(err){
                    callback(err,null);
                }else if(user){
                    callback(null,user);
                }else{
                    callback(null,null);
                }})}
        else{
            callback(null,null);
        }
    });
}
exports.getAllUser=function(callback){
    User.find({},function(err,users){
        if(err){
            callback(err,null);
        }else if (users){
            callback(null,users);
        }else{
            callback(null,null);
        }
    });
};

exports.assignDeviceToUser=function (userId,blId,callback) {
    var count;
    getUserCount(function(c){
        count=c;
        var hookN;
        if(count==0){
            hookN=1;
        }else if(count==1){
            hookN=2;
        }else{
            hookN=0;
        }
    User.update({'_id':userId},{'bluetoothId':blId,hk:hookN},function (err, affected, resp) {
        if(err){
            callback(err,null)
        }else if(affected.ok==1) {
            Blue.update({'_id':blId},{'registered':true},function(err,affected,resp){
                if(err){
                    callback(err,null)
                }else if(affected.ok==1){
                    var mh="";
                    if (hookN>0){
                        mh=", your hook is number "+hookN;
                    }
                    else{mh=", you don't have any hook assigned"}
                    var mex="bluetooth assigned" +mh;
                    callback(null,{message:mex});
                }else{
                    callback(null,{message:'bluetooth registered field was not updated'});
                }
            });
        }
        else {
            callback(null, {message: 'bluetooth not assigned to '});
        }
    });})
}



/* ********************************************************** */
/* ******************* REMINDERS FUNCTIONS ****************** */
/* ********************************************************** */

exports.getReminders=function (username,callback) {
    this.getUser(username,function (err,user) {
        if(err){
            callback(err,null);
        }else if(user){

            Reminder.find({username:username},function(err,reminders){
                if(err){
                    callback(err,null);
                }else if(reminders){
                    callback(null,reminders);
                }else{
                    callback(null,null);
                }
            })
        }

    })
};


exports.addReminder=function(username,rem,callback){
    console.log("prepare to save reminder");
    console.log(username);
    console.log(rem);
    var reminder= new Reminder();
    reminder.description=rem.description;
    reminder.username=username;
    reminder.condition=rem.condition;
    reminder.hour.from=rem.hour.from;
    reminder.hour.to=rem.hour.to;
    reminder.days.Mon=rem.days.mon;
    reminder.days.Tue=rem.days.tue;
    reminder.days.Wed=rem.days.wed;
    reminder.days.Thu=rem.days.thu;
    reminder.days.Fri=rem.days.fri;
    reminder.days.Sat=rem.days.sat;
    reminder.days.Sun=rem.days.sun;
    reminder.save(
        function (err,reminder) {
            if(err){
                callback(err,null);
            }else{
                console.log("reminder saved");
                console.log(null,reminder);
            }
        }
    )

};

exports.getReminders=function(username,callback){
    Reminder.find({'username':username},function (err,reminders) {
        if(err){
            callback(err,null);
        }else if(reminders){
            callback(null,reminders);
        }
    })
}

function getUserCount(callback){
    User.count({ bluetoothId: { $ne: null }}, function(err, count){
        callback( count );
    });
}

exports.removeReminder=function(reminderId, callback){

};
/* ********************************************************** */
/* ******************* BLUETOOTH FUNCTIONS ****************** */
/* ********************************************************** */
exports.getUnregisteredBluetooth=function(callback){
    Blue.find({'registered':false},function(err,devices){
        if(err){
            callback(err,null);
        }else if(devices){
            callback(null,devices);
        }else{
            callback(null,null);
        }
    })
};

exports.registerBluetooth=function (blID,callback) {
    Blue.findOne({'bleName':blID},function (err,device) {
        if(err){
            callback(err,null);
        }else if(device){
            var date=new Date
            Blue.update({'_id':device._id},{'updatedAt':date},function (err,affected,resp) {
                if(err){
                    callback(err,null);
                }
                console.log("BLE UPDATE");
                console.log(err);
                console.log(affected);
                console.log(resp);
                callback(null,affected);
            });

        }else{
            var newBlue = new Blue();
            newBlue.bleName=blID;
            newBlue.registered=false;
            newBlue.save(function (err) {
                if (err) {
                    callback(null,false);
                }else{
                    callback(null,true);
                }
            });
        }
    });

}
