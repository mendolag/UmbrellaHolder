/**
 * Created by josmendola on 14/04/16.
 */
var express = require('express');
var router = express.Router();

/* GET home page. */

router.get('/', function(req, res, next) {
    res.render('admin', { title: 'Umbrella holder' });
});


module.exports = router;
