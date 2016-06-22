/**
 * Created by josmendola on 14/04/16.
 */

var mongoose = require('mongoose');
var bcrypt=require('bcrypt-nodejs');
var Schema=mongoose.Schema,
    objectId=Schema.ObjectId;

var userSchema =mongoose.Schema({
    username:String,
    password:String,
    hk:Number,
    bluetoothId:{type:objectId, ref:'Bluetooth', default:null}},
    {timestamps: {createdAt: 'created_at'}}

);

userSchema.methods.generateHash=function (password) {
    return bcrypt.hashSync(password, bcrypt.genSaltSync(8), null);
};

userSchema.methods.validPassword=function(password){
    return bcrypt.compareSync(password, this.password);
};

module.exports=mongoose.model('User',userSchema);
