/**
 * Created by josmendola on 20/04/16.
 */



var host = 'https://api.forecast.io/forecast';

var apiKey = '399f7dcb47480ec0ae820d2166abeff5';
//lugano coordinates
var lat="46.0110";
var long="8.9575";


var options="units=si"

if(options) {
    var opt ="?"+options;
}
var uri=host+"/"+apiKey+"/"+lat+","+long+opt;



module.exports={
 'uri':uri,
}