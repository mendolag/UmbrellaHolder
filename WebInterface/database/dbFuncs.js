/**
 * Created by josmendola on 14/04/16.
 */

var User =require('./models/user');
var Role=require('./models/role');
var Component=require('./models/components');
var Group=require('./models/group')
var mongoose=require('mongoose');
var ObjectId = mongoose.Types.ObjectId;

/* ********************************************************** */
/* ********************* USER FUNCTIONS ********************* */
/* ********************************************************** */
exports.getUser=function(username,callback) {
    
    
};

exports.getAllUser=function(){

};

/* ********************************************************** */
/* ******************* REMINDERS FUNCTIONS ****************** */
/* ********************************************************** */

exports.getReminders=function (username,callback) {
    
};


exports.addReminder=function(username,reminder,callback){

};

exports.removeReminder=function(reminderId, callback){

};
/* ********************************************************** */
/* ******************* BLUETOOTH FUNCTIONS ****************** */
/* ********************************************************** */
exports.getUnregisteredBluetooth=function(){

};
