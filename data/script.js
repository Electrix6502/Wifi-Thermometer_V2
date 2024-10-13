
document.addEventListener('DOMContentLoaded', function () {
    Highcharts.chart('chart-temperature', {

        chart: {
            scrollablePlotArea: {
                minWidth: 700
            }
        },

        data: {
            csvURL: '/readings',
            beforeParse: function (csv) {
                return csv.replace(/\n\n/g, '\n');
            }
        },

        title: {
            text: 'Temperaturverlauf',
            align: 'left'
        },

        legend: {
            align: 'left',
            verticalAlign: 'top',
            borderWidth: 0
        },

        tooltip: {
            shared: true,
            crosshairs: true
        },

        series: [{
            name: 'temp',
            color: '#FF0000',
            lineWidth: 1,
            marker: {
                radius: 2
            }
            },{
            name: 'temp2',
            color: '#0000FF',
            lineWidth: 1,
            marker: {
                radius: 2
            }
        }]
    });
});

setInterval("location.reload(true)", 60000);

