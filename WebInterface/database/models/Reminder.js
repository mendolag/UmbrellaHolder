/**
 * Created by josmendola on 14/04/16.
 */
var mongoose = require('mongoose');

var reminderSchema =mongoose.Schema({
    name:String,
    username:String,
    condition:String,
    hour:{
        from:{ type: Number,min:0, max: 23 },
        to:{type: Number, min:0, max:23}
    },
    days:{  mon:Boolean, tue:Boolean,
            wed:Boolean, thu:Boolean,
            fri:Boolean, sat:Boolean,
            sun:Boolean},

    timestamps: Date
});

module.exports=mongoose.model('Reminder',reminderSchema);
