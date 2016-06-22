/**
 * Created by josmendola on 09/01/16.
 */

var LocalStrategy =require('passport-local').Strategy;
//var mongoose = require('mongoose');
var User =require('../database/models/User');
//var DBFunc=require('../database/dbfunc');

module.exports=function(passport){
    passport.serializeUser(function(user, done){
        done(null, user.id);
    });

    passport.deserializeUser(function(id, done){
        User.findById(id, function(err,user){

            done(err, user);
        });
    });

    //passport.serializeUser(User.serializeUser());
    //passport.deserializeUser(User.deserializeUser());

    //Local login

    passport.use('local-login',new LocalStrategy({
        usernameField:'username',
        passwordField:'password',
        passReqToCallback:true
    },
    function(req, username, password, done){
        User.findOne({'username':username},function(err, user){
            if(err) {
                return done(err);
            }
            if(!user){
                return done(null, false, req.flash('loginMessage', 'No user found'));
            }
            if(!user.validPassword(password)){
                return done(null, false, req.flash('loginMessage','Wrong password'));
            }



            //user.sessionID=req.sessionID;
            //user.save();
            //return done(null, user);

            req.login(user, function(err){
                if(err) {return done(err)}
                return  done(null, user);
            })

        });
    }
    ));

    //Function to signup
    passport.use('local-signup', new LocalStrategy({
        usernameField:'username',
        passwordField:'password',
        passReqToCallback:true
    },
    function(req, username, password, done){
        process.nextTick(function(){

            User.findOne({'username':username}, function(err, user){
                if(err){
                    return done(err);
                }
                //check if user has email
                if(user){
                    console.log('username taken');
                    return done(null, false, req.flash('signupMessage','That username is already taken.'));
                }else {
                    //create new user
                    var newUser = new User();
                    newUser.username=username;
                    newUser.password=newUser.generateHash(password);
                    newUser.bluetoothId=req.body.blId;
                    newUser.save(function(err){
                        if(err){
                            console.log("errore")
                            throw err;
                        }
                        console.log("fatto")
                        return done(null, newUser, req.flash('signupMessage', 'Registration successful'))});

                }
            });
        });
    }));
};