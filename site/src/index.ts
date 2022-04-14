import Chart from 'chart.js/auto';

async function main() {
  const getData = async (file: string) =>
    await (await (await fetch(file)).blob()).arrayBuffer();

  // const bpmData = new Uint32Array(await getData('./bpm.dat'));
  // const pulseData = new Uint32Array(await getData('./pulse.dat'));
  // const tempData = new Float32Array(await getData('./temp.dat'));
  // const o2Data = new Float32Array(await getData('./o2.dat'));

  const bpmData = [122.5, 120.1, 100.5, 113.2, 98.8, 94.5, 96.3];
  const pulseData = new Array(4)
    .fill([12, 13, 12, 17, 20, 13, 6, 78, 2, 11, 12, 21, 24, 12])
    .flat();
  const tempData = [37.3, 37.0, 36.5, 36.9, 36.8, 37.0, 37.1];
  const o2Data = [98.0, 97.7, 97.8, 98.1, 97.8, 97.8, 98.0];

  const time = [0, 10, 20, 30, 40, 50, 60]; // Minutes

  const makeChart = (
    id: string,
    title: string,
    labels: number[],
    data: number[] | Uint32Array | Float32Array,
    color: string,
    scales = {},
  ) =>
    new Chart(id, {
      type: 'line',
      data: {
        labels: labels,
        datasets: [
          {
            label: title,
            data: Array.from(data),
            fill: false,
            borderColor: color,
            tension: 0.1,
          },
        ],
      },
      options: {
        scales: {
          yAxis: scales,
        },
        maintainAspectRatio: false,
        plugins: { title: { display: true, text: title } },
      },
    });

  const bpmChart = makeChart('bpm', 'Heart Rate', time, bpmData, '#f4a261', {
    min: 0,
  });
  const pulseChart = makeChart(
    'pulse',
    'Pulse',
    Array.from({ length: pulseData.length }, (_, i) => i),
    pulseData,
    '#e76f51',
  );
  const tempChart = makeChart(
    'temp',
    'Body Temperature',
    time,
    tempData,
    '#2a9d8f',
  );
  const o2Chart = makeChart(
    'ox',
    'Blood Oxygen Concentration',
    time,
    o2Data,
    '#264653',
  );
}

window.addEventListener('load', main);
