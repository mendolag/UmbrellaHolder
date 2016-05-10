var express = require('express');
var router = express.Router();
var hf=require('../forcast/funcs');
var dbFuncs=require('../database/dbFuncs');



router.get('/getUserWithBl/:blID',function (req,res) {
  var blId=req.params.blID;
  console.log(blId);
  dbFuncs.getUserByBlue(blId,function (err, user) {
    console.log("ble");
    if(err) {
      var message = {message: "Database error", error: {status: res.statusCode, stack: err}}
      resError(req, res, message);
    }else if (user){
      console.log(user);
    }else{
      dbFuncs.registerBluetooth(blId,function (err,registered) {
        if(err){
          var message = {message: "Bluetooth registration error", error: {status: res.statusCode, stack: err}}
          resError(req, res, message);
        }else if(!registered){
          var message = {message: "Bluetooth ID exists", error: {status: res.statusCode, stack: err}}
          resError(req, res, message);
        }else if(registered){
          res.send("New Bluetooth registered");
        }
      })
    }
  })
})

router.get('/getUnregisteredBluetooth',function(req,res){
  console.log("getUnregistered");
  dbFuncs.getUnregisteredBluetooth(function (err,devices) {
    if(err){
      throw err;
    }else if(devices){
      res.send({devFound:false,devices:devices});
    }else{
      res.send({devFound:false, message:'make sure your bluetooth is registered'})
    }
  })
});

router.get('/todayForecast',function(req,res){
  hf.forcastRequest(function(err,req,body){
    if(err){
      console.log("error: "+err);
    }else{
      console.log(body);
      res.send(body);

    }
  })
});

function resError(req,res,error){
  res.render('error',error);
}

module.exports = router;
