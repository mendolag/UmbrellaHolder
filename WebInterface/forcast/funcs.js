/**
 * Created by josmendola on 20/04/16.
 */


var request=require('request');
var uri=require('./params').uri;



exports.forcastRequest=function(callback) {
    request(uri,  function (error, response, body) {
        if (error){
            callback(error,null,null);
        }if(response){
            var filteredBody=forecastFilter(JSON.parse(body));
            callback(null,response,filteredBody);
        }
    });
}

function avarageToday(hourly) {
    var summary=hourly.summary;
    var icon=hourly.icon;
    var data=hourly.data;
    var dataLen=data.length;
    var today=undefined;
    if(dataLen>0){
        var from=convertTimestamp(data[0].time);
        var to=convertTimestamp(data[1].time);
        var avgProb=0;
        var avgWSpeed=0;
        var avgHum=0;
        var minTemp=1000;
        var maxTemp=-1000;

        var i=0;
        while (true) {

            var to = convertTimestamp(data[i].time);

            var d = data[i];
            if (minTemp > d.temperature) {
                minTemp = d.temperature;
            }
            if (maxTemp < d.temperature) {
                maxTemp = d.temperature;
            }
            avgProb += d.precipProbability;
            avgWSpeed += d.windSpeed;
            avgHum += d.humidity;
            if (to.day!=from.day){
                break;
            }
            i++;
        }

        avgProb=avgProb/i;
        avgWSpeed=avgWSpeed/i;
        avgHum=avgHum/i;

        var daily={
            summary:summary,
            icon:icon,
            min:minTemp,
            max:maxTemp,
            from:from,
            to:to,
            avgProb:Math.floor(avgProb*100),
            avgWindSpeed:avgWSpeed,
            avgHumidity:Math.floor(avgHum*100)
        }
        
    }
    return daily;
}

function forecastFilter(forecastBody) {
    var currentDay="error";
    var currentTime="error";
    var myForecast = {current: null, nextHour: null, today: null}
    if (forecastBody.currently){
        var res = forecastBody.currently;
        var ts = convertTimestamp(res.time);
        currentDay=ts.day+"."+ts.month+"."+ts.year;
        currentTime=ts.hour + ':' + ts.min + ' ' + ts.ampm;
        var currentTime = {
            summary: res.summary,
            icon:res.icon,
            date:currentDay,
            time:currentTime,
            temp:res.temperature,
            windSpeed:res.windSpeed,
            humidity:Math.floor(res.humidity*100),
            precProb:Math.floor(res.precipProbability*100)

        };
        myForecast.current=currentTime;
        var h=1;
        if(ts.min>30){
            h++
        }
        res=forecastBody.hourly.data[h];
        ts=convertTimestamp(res.time);
        var nextHour={
            summary:res.summary,
            icon:res.icon,
            date:ts.day+"."+ts.month+"."+ts.year,
            time:ts.hour + ':' + ts.min + ' ' + ts.ampm,
            temp:res.temperature,
            windSpeed:res.windSpeed,
            humidity:Math.floor(res.humidity*100),
            precProb:Math.floor(res.precipProbability*100)
        }

        myForecast.nextHour=nextHour;




        res=forecastBody.hourly;

        myForecast.today=avarageToday(res);


    }
    return myForecast
}
// time = yyyy + '-' + mm + '-' + dd + ', ' + h + ':' + min + ' ' + ampm;



function convertTimestamp(timestamp){
    var d = new Date(timestamp * 1000),	// Convert the passed timestamp to milliseconds
        yyyy = d.getFullYear(),
        mm = ('0' + (d.getMonth() + 1)).slice(-2),	// Months are zero based. Add leading 0.
        dd = ('0' + d.getDate()).slice(-2),			// Add leading 0.
        hh = d.getHours(),
        h = hh,
        min = ('0' + d.getMinutes()).slice(-2),		// Add leading 0.
        ampm = 'AM',
        time;

    if (hh > 12) {
        h = hh - 12;
        ampm = 'PM';
    } else if (hh === 12) {
        h = 12;
        ampm = 'PM';
    } else if (hh == 0) {
        h = 12;
    }

    // ie: 2013-02-18, 8:35 AM
    //time = yyyy + '-' + mm + '-' + dd + ', ' + h + ':' + min + ' ' + ampm;
    time={year:yyyy,month:mm,day:dd,hour:h,min:min,ampm:ampm};
    return time;
}

module.exports;
