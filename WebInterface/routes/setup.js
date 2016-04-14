/**
 * Created by josmendola on 14/04/16.
 */
var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
    console.log(next);
    console.log('index');
    res.render('index', { title: 'setup' });
});


module.exports = router;
