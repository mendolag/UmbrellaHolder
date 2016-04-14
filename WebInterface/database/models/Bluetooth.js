/**
 * Created by josmendola on 14/04/16.
 */

var mongoose = require('mongoose');


var bleSchema =mongoose.Schema({
    bleName:String,
    registered:Boolean,
    timestamp:true
})

module.exports=mongoose.model('Bluetooth',bleSchema);
    