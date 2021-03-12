function getColumnOpts(table) {
    let columns = [];
    table.find('thead th').each( function( i, el ) {
        let column = {
            data: $(this).data('field') || $(this).text() || null,
            defaultContent: ''
        };
        columns.push(column);
    });
    return columns;
}

function debugTableRefresh(){
    let table = $('#debug');
    let _select = $('#debug_files');
    table.DataTable().clear();
    $.getJSON( 'log_files.json', function(data) {
        if (data.debug){
            let _val =  _select.val();
            _select.find('option').remove();
            for (let i = data.debug.length -1; i >= 0; i--){
                _select.append(new Option(data.debug[i], data.debug[i]));
            }
            if (_val){ _select.val(_val); }
        }
    })
    .always(function() {
        $.get(_select.val(), function(response) {
            table.DataTable().rows.add($.csv.toObjects(response));
        })
        .always(function() {
            table.DataTable().draw();
        });
    });
}

function logTableRefresh(){
    let table = $('#log');
    let _select = $('#log_files');
    table.DataTable().clear();
    $.getJSON( 'log_files.json', function(data) {
        if (data.log){
            let _val =  _select.val();
            _select.find('option').remove();
            console.log(data.log.length);
            for (let i = data.log.length-1; i >= 0; i--){
                console.log(i);
                console.log(data.log[i]);
                _select.append(new Option(data.log[i], data.log[i]));
            }
            if (_val){ _select.val(_val); }
        }
    })
    .always(function() {
        $.get(_select.val(), function(response) {
            table.DataTable().rows.add($.csv.toObjects(response));
        })
        .always(function() {
            table.DataTable().draw();
        });
    });
}

function initLogTable(){
    let table = $('#log');
    table.DataTable( {
        "order": [[ 0, "desc" ]],
        "columns":  getColumnOpts(table),
        "fixedHeader": true,
        dom: 'lBfrtpi', //        dom: 'Bfrtip',lBrtip
        buttons: [
            'copyHtml5','excelHtml5','csvHtml5',
            {
                text: 'Reload',
                action: function ( e, dt, node, config ) {
                    logTableRefresh();
                }
            }
        ]
    } );
}

function initDebugTable(){
    let table = $('#debug');
    table.DataTable( {
        "order": [[ 0, "desc" ]],
        "columns":  getColumnOpts(table),
        "fixedHeader": true,
        dom: 'lBfrtpi', //        dom: 'Bfrtip',lBrtip
        buttons: [
            'copyHtml5','excelHtml5','csvHtml5',
            {
                text: 'Reload',
                action: function ( e, dt, node, config ) {
                    debugTableRefresh();
                }
            }
        ]
    } );
}

let gauges = {};

function refreshGauges(){
    $.get("current.csv", function(response) {
        let data = $.csv.toObjects(response)[0];

        //==========================
        gaugeSet(gauges.air_p, data.air_p);
        let _txt = '';
        if (data.air_t) _txt += data.air_t + '°C; ';
        if (data.air_h) _txt += data.air_h + '%;';
        $('#g_air_p_text2').text(_txt);

        //==========================
        gaugeSet(gauges.w_depth, data.w_depth);
        _txt = '';
        if (data.w_temp) _txt += data.w_temp + '°C';
        $('#g_w_depth_text2').text(_txt);

        //==========================
        data.cog = Math.round(data.cog);
        let heading;
        if (data.head_t){
            data.head_t = Math.round(data.head_t);
            gauges.cog.set([data.cog, data.head_t]);
            $('#g_cog_text').html(data.cog + '/<span class="gauge_second_arrow">' + data.head_t + '</span>');
        }
        else{
            gauges.cog.set(data.cog);
            $('#g_cog_text').text(data.cog);
        }
        _txt = '';
        //if (data.mcog) _txt += Math.round(data.mcog) + '°M; '; //knowing variation is enought
        if (data.varia) _txt += data.varia + '° var';
        $('#g_cog_text2').text(_txt);

        //==========================
        gaugeSet(gauges.bat_charge, data.bat_charge);
        $('#g_bat_charge_text2').text(data.bat_volt + 'V; ' + data.bat_cur + 'A; '  + data.pow_used + 'w/h' );

        //==========================
        if (data.twa){
            gauges.wind.set([data.awa, data.twa]);
            $('#g_wind_text').html(Math.abs(data.awa) + '/<span class="gauge_second_arrow">' + Math.abs(data.twa) + '</span>');
            $('#g_wind_text3').html(data.aws + '/<span class="gauge_second_arrow">' + data.tws + '</span>kn');
        }
        else{
            gauges.cog.set(data.awa);
            $('#g_wind_text').text(Math.abs(data.awa));
            $('#g_wind_text3').html(data.aws);
        }
        _txt = '';
        if (data.twd) _txt += data.twd + '°';
        $('#g_wind_text2').text(_txt);

        //==========================
        gaugeSet(gauges.heel, -1 * data.heel);
        _txt = '';
        if (data.trim) _txt += 'trim: ' + data.trim + '°';
        $('#g_heel_text2').text(_txt);

        //==========================
        gaugeSet(gauges.polar, data.boat_to_polar);
        $('#g_polar_text2').html('boat/polar<br>' + data.stw + '/' + data.polar_stw);

        //==========================
        if (data.stw){
            gauges.sog.set([data.sog, data.stw]);
            $('#g_sog_text').html(data.sog + '/<span class="gauge_second_arrow">' + data.stw + '</span>');
        }
        else{
            gauges.sog.set(data.sog);
            $('#g_sog_text').text(data.stw);
        }
        if (data.polar_stw) {
            $('#g_sog_text2').html('<br>polar:<br>' + data.polar_stw + 'kn; ' + data.boat_to_polar + '%');
        }

        //==========================
        $('#gps_src').text(data.gps_src);
        $('#head_src').text(data.head_src);
        $('#att_src').text(data.att_src);
        $('#gps_lat').text(data.lat);
        $('#gps_lon').text(data.lon);
        $('#device_time').text(data.time);
        $('#gps_time').text(data.gps_time);
        $('#uptime').text(data.uptime);
    });
}

function gaugeSet(gauge, val){
    let _parent = $("#"+gauge.canvas.id).closest('.gauge_cell');
    _parent.find('.g_na').remove();
    _parent.find('.g_out_of_range').remove();
    val = parseFloat(val);
    if (isNaN(val)){
        gauge.set(gauge.minValue);
        _parent.find('.gauge_label').hide();
        _parent.find('.gauge_label').after($('<span class="g_na">N/A</span>'));
        return;
    }

    if (val > gauge.maxValue || val < gauge.minValue){
        _parent.find('.gauge_label').hide();
        _parent.find('.gauge_label').after($('<span class="g_out_of_range">'+val+'</span>'));
    }
    else{
        _parent.find('.gauge_label').show();
    }

//    let numVal = parseFloat(val);
    gauge.set(val);
}

function InitGauges(){
    function pointer(){
        return {
                length: 0.46,
                strokeWidth: 0.013,
           };
    }

    let animationSpeed = 128;

    //Weather
    let opts = {
      angle: -0.25,
      lineWidth: 0.07,
      radiusScale: 1,
      pointer: pointer(),
      limitMax: true,
      limitMin: true,
      highDpiSupport: true,
      staticZones: [
         {strokeStyle: "#CCCCCC", min: 960, max: 1010},
         {strokeStyle: "#FFDD00", min: 1010, max: 1060},
      ],
      staticLabels: {font: "10px",  labels: [970, 990, 1010, 1030, 1050]},
      renderTicks: {
          divisions: 10,
          divColor: '#333333',
          subDivisions: 2,
          subLength: 0.5,
          subWidth: 0.3,
          subColor: '#333333',
        }
    };
    let gauge = new Gauge(document.getElementById('g_air_p')).setOptions(opts);
    gauge.setTextField(document.getElementById("g_air_p_text"));
    gauge.maxValue = 1060;
    gauge.setMinValue(960);
    gauge.set(gauge.minValue);
    gauge.animationSpeed = animationSpeed;
    gauges.air_p = gauge;

    //cog
    opts = {
      angle: -0.5,
      lineWidth: 0.07,
      radiusScale: 1,
      pointer: pointer(),
      limitMax: true,
      limitMin: true,
      highDpiSupport: true,
        staticZones: [
           {strokeStyle: "#F03E3E", min: 0, max: 180},
           {strokeStyle: "#30B32D", min: 180, max: 360},
        ],
      renderTicks: {
          divisions: 12,
          divColor: '#333333',
          subDivisions: 6,
          subLength: 0.5,
          subWidth: 0.3,
          subColor: '#333333',
        }
    };
    opts.pointer.angle_offset=180;
    gauge = new Gauge(document.getElementById('g_cog')).setOptions(opts);
    gauge.maxValue = 360;
    gauge.setMinValue(0);
    gauge.set(gauge.minValue);
    gauge.animationSpeed = 1;
    gauges.cog = gauge;

    //depth
    opts = {
      angle: 0.25,
      lineWidth: 0.14,
      radiusScale: 1,
      pointer:pointer(),
      limitMax: true,
      limitMin: true,
      generateGradient: false,
      highDpiSupport: true,
      staticZones: [
         {strokeStyle: "#F03E3E", min: 0, max: 3},
         {strokeStyle: "#FFDD00", min: 3, max: 10},
         {strokeStyle: "#30B32D", min: 10, max: 30},
      ],
      staticLabels: {font: "10px",  labels: [0,5,10,15,20,25,30]},
      renderTicks: {
          divisions: 3,
          divColor: '#333333',
          subDivisions: 2,
          subLength: 0.3,
          subWidth: 0.6,
          subColor: '#333333',
        }
    };
    gauge = new Gauge(document.getElementById('g_w_depth')).setOptions(opts);
    gauge.setTextField(document.getElementById("g_w_depth_text"), 1);
    gauge.maxValue = 30;
    gauge.setMinValue(0);
    gauge.set(gauge.minValue);
    gauge.animationSpeed = animationSpeed;
    gauges.w_depth = gauge;

    //battery
    opts = {
      angle: 0.25,
      lineWidth: 0.07,
      radiusScale: 1,
      pointer:pointer(),
      limitMax: true,
      limitMin: true,
      highDpiSupport: true,
      generateGradient: true,
      percentColors : [[0.0, "#F03E3E"], [0.5, "#FFDD00"], [1.0, "#30B32D"]],
      staticLabels: {font: "10px",   labels: [0,20,40,60,80,100]},
      renderTicks: {
          divisions: 10,
          divColor: '#333333',
        }
    };
    gauge = new Gauge(document.getElementById('g_bat_charge')).setOptions(opts);
    gauge.setTextField(document.getElementById("g_bat_charge_text"), 0);
    gauge.maxValue = 100;
    gauge.setMinValue(0);
    gauge.set(gauge.minValue);
    gauge.animationSpeed = animationSpeed;
    gauges.bat_charge = gauge;

    //wind
    opts = {
      angle: -0.5,
      lineWidth: 0.07,
      radiusScale: 1,
      pointer: pointer(),
      limitMax: true,
      limitMin: true,
      highDpiSupport: true,
        staticZones: [
           {strokeStyle: "#CCCCCC", min: -180, max: -150},
           {strokeStyle: "#F03E3E", min: -150, max: -30},
           {strokeStyle: "#CCCCCC", min: -30, max: 30},
           {strokeStyle: "#30B32D", min: 30, max: 150},
           {strokeStyle: "#CCCCCC", min: 150, max: 180},
        ],
      renderTicks: {
          divisions: 12,
          divColor: '#333333',
          subDivisions: 6,
          subLength: 0.5,
          subWidth: 0.3,
          subColor: '#333333',
        }
    };
    gauge = new Gauge(document.getElementById('g_wind')).setOptions(opts);
    gauge.maxValue = 180;
    gauge.setMinValue(-180);
    gauge.set(gauge.minValue);
    gauge.animationSpeed = 1;
    gauges.wind = gauge;

    //heel
    opts = {
      angle: 0.25,
      lineWidth: 0.07,
      radiusScale: 1,
      pointer:pointer(),
      limitMax: true,
      limitMin: true,
      generateGradient: false,
      highDpiSupport: true,
      staticZones: [
           {strokeStyle: "#F03E3E", min: -40, max: 0},
           {strokeStyle: "#30B32D", min: 0, max: 40},
      ],
      staticLabels: {font: "10px",  labels: [-40,-30,-20,-10,0,10,20,30,40]},
      renderTicks: {
          divisions: 8,
          divColor: '#333333',
          subDivisions: 2,
          subLength: 0.5,
          subWidth: 0.3,
          subColor: '#333333',
        }
    };
    gauge = new Gauge(document.getElementById('g_heel')).setOptions(opts);
    gauge.setTextField(document.getElementById("g_heel_text"), 0);
    gauge.maxValue = 40;
    gauge.setMinValue(-40);
    gauge.set(gauge.minValue);
    gauge.animationSpeed = animationSpeed;
    gauges.heel = gauge;

     //Polar
    opts = {
      angle: 0.25,
      lineWidth: 0.07,
      radiusScale: 1,
      pointer:pointer(),
      limitMax: true,
      limitMin: true,
      generateGradient: true,
      percentColors : [[0.0, "#F03E3E"], [0.5, "#FFDD00"], [1.0, "#30B32D"]],
      highDpiSupport: true,
      staticLabels: {font: "10px",   labels: [50,60,70,80,90,100]},
      renderTicks: {
          divisions: 5,
          divColor: '#333333',
          subDivisions: 2,
          subLength: 0.5,
          subWidth: 0.3,
          subColor: '#333333',
        }
    };
    gauge = new Gauge(document.getElementById('g_polar')).setOptions(opts);
    gauge.setTextField(document.getElementById("g_polar_text"), 0);
    gauge.maxValue = 100;
    gauge.setMinValue(50);
    gauge.set(gauge.minValue);
    gauge.animationSpeed = animationSpeed;
    gauges.polar = gauge;

    //SOG STW
    opts = {
      angle: -0.25,
      lineWidth: 0.07,
      radiusScale: 1,
      pointer: pointer(),
      limitMax: true,
      limitMin: true,
      highDpiSupport: true,
      staticLabels: {font: "10px",  labels: [0,1,2,3,4,5,6,7,8]},
      staticZones: [
           {strokeStyle: "#FFDD00", min: 0, max: 5},
           {strokeStyle: "#30B32D", min: 5, max: 8},
      ],
      renderTicks: {
          divisions: 8,
          divColor: '#333333',
          subDivisions: 2,
          subLength: 0.5,
          subWidth: 0.5,
          subColor: '#333333',
        }
    };
    gauge = new Gauge(document.getElementById('g_sog')).setOptions(opts);
    gauge.maxValue = 8;
    gauge.setMinValue(0);
    gauge.set(gauge.minValue);
    gauge.animationSpeed = animationSpeed;
    gauges.sog = gauge;
    refreshGauges();
}

function _refreshGauges(data){
    gPreasure(data.air_p);
    console.log(data);
}

$(document).ready(function() {
    initLogTable();
    initDebugTable();
    InitGauges();
    let gaugeRefreshInterval = setInterval(refreshGauges, 5000); //refresh each 5 sec

    $('#pills-log-tab').click(logTableRefresh);
    $('#pills-debug-tab').click(debugTableRefresh);
    $('#log_files').change(logTableRefresh);
    $('#debug_files').change(debugTableRefresh);

    $('#gauge_refresh').change(function(){
        let val = parseInt($(this).val());
        if (1 <= val && val <= 10){
            clearInterval(gaugeRefreshInterval);
            gaugeRefreshInterval = setInterval(refreshGauges, val * 1000);
        }
    });
} );
