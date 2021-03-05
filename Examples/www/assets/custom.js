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

$(document).ready(function() {
    initTable($('#log'), "example1.csv");
    initTable($('#debug'), "example2.csv");
} );
