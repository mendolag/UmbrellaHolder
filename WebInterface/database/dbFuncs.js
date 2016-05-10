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

exports.getUserByBlue=function(bleId,callback){
    User.findOne({'bluetootId':bleId},function (err ,user) {
        if(err){
            callback(err,null);
        }else if(user){
            callback(null,user);
        }else{
            callback(null,null);
        }
    });
};

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



/* ********************************************************** */
/* ******************* REMINDERS FUNCTIONS ****************** */
/* ********************************************************** */

exports.getReminders=function (username,callback) {
    this.getUser(username,function (err,user) {
        if(err){
            callback(err,null);
        }else if(user){
            user.populate({path:'bluetoothId'})
        }

    })
};


exports.addReminder=function(username,reminder,callback){

};

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
            callback(null,false);
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
