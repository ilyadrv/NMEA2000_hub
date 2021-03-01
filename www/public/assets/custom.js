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

$(document).ready(function() {
    $('#log').DataTable( {
        "ajax":{url:"example1.json",dataSrc:""},
        "order": [[ 0, "desc" ]],
        "columns":  getColumnOpts($('#log')),
    } );

    $('#debug').DataTable( {
        "ajax":{url:"example2.json",dataSrc:""},
        "order": [[ 0, "desc" ]],
        "columns":  getColumnOpts($('#debug')),
    } );
} );
