/**
 * Created by josmendola on 14/04/16.
 */
var mongoose = require('mongoose');

var reminderSchema =mongoose.Schema({
    description:String,
    username:String,
    condition:String,
    hour:{
        from:{ type: Number,min:0, max: 24 },
        to:{type: Number, min:0, max:24}
    },
    days:{  Mon:Boolean, Tue:Boolean,
            Wed:Boolean, Thu:Boolean,
            Fri:Boolean, Sat:Boolean,
            Sun:Boolean},},

{timestamps: { createdAt: 'created_at' } }
);

module.exports=mongoose.model('Reminder',reminderSchema);
