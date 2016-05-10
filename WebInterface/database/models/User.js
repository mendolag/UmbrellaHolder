/**
 * Created by josmendola on 14/04/16.
 */

var mongoose = require('mongoose');
var Schema=mongoose.Schema,
    objectId=Schema.ObjectId;

var userSchema =mongoose.Schema({
    email:String,
    username:String,
    bluetoothId:{type:objectId, ref:'Bluetooth'},
    reminders:[{type:objectId,ref:'Reminder'}],
    timestamps: Date
})

module.exports=mongoose.model('User',userSchema);
