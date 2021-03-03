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

function initTable(table, file){
    $.get(file, function(response) {
        table.DataTable( {
             data: $.csv.toObjects(response),
            "order": [[ 0, "desc" ]],
            "columns":  getColumnOpts(table),
        } );
    });
}

$(document).ready(function() {
    initTable($('#log'), "example1.csv");
    initTable($('#debug'), "example2.csv");
} );
