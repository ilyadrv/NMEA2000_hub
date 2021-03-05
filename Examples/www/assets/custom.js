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

function csvToTable(table, file){
    table.DataTable().clear();
    $.get(file, function(response) {
        table.DataTable().rows.add($.csv.toObjects(response)).draw();
    });
}

function initTable(table, file){
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
                    csvToTable(table, file);
                }
            }
        ]
    } );
    csvToTable(table, file);
}

let gauges = {};

function compassToGauge(val){
    if (val > 360 || val < 0) return null;
    if (val > 180) return val - 180;
    if (val <= 180) return val + 180;
}

function refreshGauges(){
    $.get("example3.csv", function(response) {
        let data = $.csv.toObjects(response)[0];

        gaugeSet(gauges.air_p, data.air_p);
        gaugeSet(gauges.w_depth, data.w_depth);

        data.cog = Math.round(data.cog);
        gaugeSet(gauges.cog, compassToGauge(data.cog));  // gauges.cog.set([data.mcog,data.cog]);
        $('#g_cog_text').text(data.cog);
        let _txt = '';
        if (data.mcog) _txt += Math.round(data.mcog) + '° M; ';
        if (data.varia) _txt += data.varia + '° var;';
        $('#g_cog_text2').text(_txt);

        gaugeSet(gauges.bat_charge, data.bat_charge);
        $('#g_bat_charge_text2').text(data.bat_volt + 'V; ' + data.bat_cur + 'A; '  + data.pow_used + 'w/h;' );
    });
}

function gaugeSet(gauge, val){
    if (!val.length){
        gauge.set(val);
        return;
    }

    let _parent = $("#"+gauge.canvas.id).closest('.gauge_cell');
    _parent.find('.g_out_of_range').remove();

    if (val > gauge.maxValue || val < gauge.minValue){
        _parent.find('.gauge_label').hide();
        _parent.find('.gauge_label').after($('<span class="g_out_of_range">'+val+'</span>'));
    }
    else{
        _parent.find('.gauge_label').show();
    }
    gauge.set(val);
}

function InitGauges(){
    let pointer = {
        length: 0.46,
        strokeWidth: 0.013,
        color: '#000000'
   };

    //air pressure
    let opts = {
      angle: -0.25,
      lineWidth: 0.07,
      radiusScale: 1,
      pointer: pointer,
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
          subDivisions: 10,
          subLength: 0.5,
          subWidth: 0.3,
          subColor: '#666666',
        }
    };
    let gauge = new Gauge(document.getElementById('g_air_p')).setOptions(opts);
    gauge.setTextField(document.getElementById("g_air_p_text"));
    gauge.maxValue = 1060;
    gauge.setMinValue(960);
    gauge.set(gauge.minValue);
    gauges.air_p = gauge;

    //cog
    opts = {
      angle: -0.5,
      lineWidth: 0.07,
      radiusScale: 1,
      pointer: pointer,
      limitMax: true,
      limitMin: true,
      highDpiSupport: true,
        staticZones: [
           {strokeStyle: "#F03E3E", min: 0, max: 180},
           {strokeStyle: "#30B32D", min: 180, max: 360},
        ],
      renderTicks: {
          divisions: 8,
          divColor: '#333333',
          subDivisions: 9,
          subLength: 0.5,
          subWidth: 0.3,
          subColor: '#666666',
        }
    };
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
      pointer:pointer,
      limitMax: true,
      limitMin: true,
      generateGradient: false,
      highDpiSupport: true,
      staticZones: [
         {strokeStyle: "#F03E3E", min: 0, max: 3},
         {strokeStyle: "#FFDD00", min: 3, max: 10},
         {strokeStyle: "#30B32D", min: 10, max: 30},
      ],
      staticLabels: {font: "10px",  labels: [0,10,20,30]},
      renderTicks: {
          divisions: 3,
          divColor: '#333333',
          subDivisions: 10,
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
    gauges.w_depth = gauge;

    //battery
    opts = {
      angle: 0.25,
      lineWidth: 0.07,
      radiusScale: 1,
      pointer:pointer,
      limitMax: true,
      limitMin: true,
      generateGradient: false,
      highDpiSupport: true,
      staticZones: [
         {strokeStyle: "#F03E3E", min: 0, max: 33},
         {strokeStyle: "#FFDD00", min: 33, max: 66},
         {strokeStyle: "#30B32D", min: 66, max: 100},
      ],
      staticLabels: {font: "10px",   labels: [0, 50,100]},
      renderTicks: {
          divisions: 2,
          divColor: '#333333',
          subDivisions: 5,
          subLength: 0.3,
          subWidth: 0.6,
          subColor: '#333333',
        }
    };
    gauge = new Gauge(document.getElementById('g_bat_charge')).setOptions(opts);
    gauge.setTextField(document.getElementById("g_bat_charge_text"), 0);
    gauge.maxValue = 100;
    gauge.setMinValue(0);
    gauge.set(gauge.minValue);
    gauges.bat_charge = gauge;


    refreshGauges();
}

function _refreshGauges(data){
    gPreasure(data.air_p);
    console.log(data);
}

$(document).ready(function() {
    initTable($('#log'), "example1.csv");
    initTable($('#debug'), "example2.csv");
    InitGauges();
    setInterval(refreshGauges, 5000); //refresh each 5 sec
} );
