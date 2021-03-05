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

function refreshGauges(){
    $.get("example3.csv", function(response) {
        let data = $.csv.toObjects(response)[0];
        gauges.air_p.set(data.air_p);
    });
}

function InitGauges(){
    //air preasure
    let opts = {
      angle: -0.25,
      lineWidth: 0.07,
      radiusScale: 1,
      pointer: {
        length: 0.46,
        strokeWidth: 0.013,
        color: '#000000'
      },
      limitMax: false,
      limitMin: false,
      colorStart: '#6FADCF',
      colorStop: '#8FC0DA',
      strokeColor: '#E0E0E0',
      generateGradient: true,
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
    gauge.animationSpeed = 32;
    gauge.set(960);
    gauges.air_p = gauge;


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
