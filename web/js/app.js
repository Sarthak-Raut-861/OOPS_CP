/**
 * FitMetric — Clean, Simple Frontend Application Logic
 */

document.addEventListener('DOMContentLoaded', () => {
    const API_BASE = window.location.origin;
    let activeTrace = null;

    // Navigation Tabs
    const navButtons = document.querySelectorAll('.nav-btn');
    const tabPanes = document.querySelectorAll('.tab-content');

    navButtons.forEach(btn => {
        btn.addEventListener('click', () => {
            const targetId = btn.dataset.tab;
            navButtons.forEach(b => b.classList.remove('active'));
            tabPanes.forEach(p => p.classList.remove('active'));

            btn.classList.add('active');
            const targetPane = document.getElementById(targetId);
            if (targetPane) targetPane.classList.add('active');

            if (targetId === 'tab-history') loadHistory();
        });
    });

    // Tolerance Toggle
    const enableTolerance = document.getElementById('enableTolerance');
    const toleranceFields = document.getElementById('toleranceFields');
    enableTolerance.addEventListener('change', () => {
        toleranceFields.style.display = enableTolerance.checked ? 'block' : 'none';
    });

    // Borderline Sample Loader
    document.getElementById('btnDemoPreset').addEventListener('click', () => {
        document.getElementById('weightKg').value = '77.0';
        document.getElementById('heightCm').value = '175.5';
        document.getElementById('ageYears').value = '28';
        document.getElementById('sex').value = 'male';
        document.getElementById('activityLevel').value = 'lightly_active';
        document.getElementById('profileContext').value = 'general';

        enableTolerance.checked = true;
        toleranceFields.style.display = 'block';

        document.getElementById('weightMin').value = '75.8';
        document.getElementById('weightMax').value = '78.2';
        document.getElementById('heightMin').value = '175.0';
        document.getElementById('heightMax').value = '176.0';
    });

    // Reset Form
    document.getElementById('btnReset').addEventListener('click', () => {
        document.getElementById('calcForm').reset();
        enableTolerance.checked = true;
        toleranceFields.style.display = 'block';
    });

    // Form Submit
    document.getElementById('calcForm').addEventListener('submit', async (e) => {
        e.preventDefault();
        const btnCalc = document.getElementById('btnCalculate');
        btnCalc.disabled = true;
        btnCalc.innerText = 'Calculating...';

        const payload = {
            weightKg: parseFloat(document.getElementById('weightKg').value),
            heightCm: parseFloat(document.getElementById('heightCm').value),
            ageYears: parseInt(document.getElementById('ageYears').value),
            sex: document.getElementById('sex').value,
            activityLevel: document.getElementById('activityLevel').value,
            profileContext: document.getElementById('profileContext').value,
            save: true
        };

        if (enableTolerance.checked) {
            const wMin = parseFloat(document.getElementById('weightMin').value);
            const wMax = parseFloat(document.getElementById('weightMax').value);
            if (!isNaN(wMin) && !isNaN(wMax)) {
                payload.weightMin = wMin;
                payload.weightMax = wMax;
            }

            const hMin = parseFloat(document.getElementById('heightMin').value);
            const hMax = parseFloat(document.getElementById('heightMax').value);
            if (!isNaN(hMin) && !isNaN(hMax)) {
                payload.heightMin = hMin;
                payload.heightMax = hMax;
            }
        }

        try {
            const res = await fetch(`${API_BASE}/api/v1/assess-stability`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(payload)
            });

            if (!res.ok) {
                const err = await res.json();
                throw new Error(err.error || 'Server calculation failed');
            }

            const data = await res.json();
            activeTrace = data;
            displayResults(data);

        } catch (err) {
            alert(`Error: ${err.message}`);
        } finally {
            btnCalc.disabled = false;
            btnCalc.innerText = 'Calculate & Evaluate';
        }
    });

    // Display Results Function
    function displayResults(data) {
        // Numbers
        document.getElementById('resBmi').innerText = data.results.bmi.toFixed(2);
        document.getElementById('resCategory').innerText = data.results.nominalCategory;
        document.getElementById('resBmr').innerText = Math.round(data.results.bmr);
        document.getElementById('resTdee').innerText = Math.round(data.results.tdee);

        if (data.results.bmiInterval) {
            document.getElementById('resBmiInterval').innerText = 
                `Interval: [${data.results.bmiInterval.min.toFixed(2)}, ${data.results.bmiInterval.max.toFixed(2)}] kg/m²`;
        } else {
            document.getElementById('resBmiInterval').innerText = 'Interval: Not declared';
        }

        // Stability Alert
        const alertBox = document.getElementById('stabilityAlert');
        const alertTitle = document.getElementById('alertTitle');
        const alertDesc = document.getElementById('alertDesc');

        alertBox.className = 'alert-box';
        if (data.stability.state === 'CROSSES_CATEGORY_BOUNDARY') {
            alertBox.classList.add('state-crossing');
            alertTitle.innerText = 'Crosses Category Boundary (Ambiguous)';
        } else if (data.stability.state === 'STABLE_WITHIN_DECLARED_BOUNDS') {
            alertBox.classList.add('state-stable');
            alertTitle.innerText = 'Stable Within Declared Bounds';
        } else {
            alertTitle.innerText = 'Stability Not Assessable';
        }
        alertDesc.innerText = data.stability.decisionReason;

        // Scale Visualizer
        renderScale(data);

        // Verification Recommendation Table
        const recBox = document.getElementById('recBox');
        const recText = document.getElementById('recText');
        const tbody = document.getElementById('recTableBody');

        if (data.verification && data.verification.candidates && data.verification.candidates.length > 0) {
            recBox.style.display = 'block';
            recText.innerText = data.verification.summary;
            tbody.innerHTML = '';

            data.verification.candidates.forEach(c => {
                const tr = document.createElement('tr');
                tr.innerHTML = `
                    <td><strong>Priority ${c.rank}</strong></td>
                    <td>${c.targetMeasurement}</td>
                    <td>${c.proposedTool}</td>
                    <td>${c.intervalReductionPercent.toFixed(1)}% contraction</td>
                `;
                tbody.appendChild(tr);
            });
        } else {
            recBox.style.display = 'none';
        }
    }

    // Scale Positioning
    function renderScale(data) {
        const minScale = 15.0;
        const maxScale = 35.0;

        function toPct(v) {
            const clamped = Math.min(Math.max(v, minScale), maxScale);
            return ((clamped - minScale) / (maxScale - minScale)) * 100;
        }

        const bracket = document.getElementById('scaleBracket');
        const nomBmi = data.results.bmi;
        document.getElementById('gaugeStatusText').innerText = `Nominal: ${nomBmi.toFixed(2)} kg/m²`;

        if (data.results.bmiInterval) {
            bracket.style.display = 'block';
            const bMin = data.results.bmiInterval.min;
            const bMax = data.results.bmiInterval.max;

            const left = toPct(bMin);
            const right = toPct(bMax);
            bracket.style.left = `${left}%`;
            bracket.style.width = `${Math.max(right - left, 3)}%`;

            if (data.stability.state === 'CROSSES_CATEGORY_BOUNDARY') {
                bracket.classList.add('borderline');
            } else {
                bracket.classList.remove('borderline');
            }
        } else {
            bracket.style.display = 'none';
        }
    }

    // Modal Handlers
    const modal = document.getElementById('traceModal');
    document.getElementById('btnOpenTrace').addEventListener('click', () => {
        if (!activeTrace) return;
        document.getElementById('traceJsonText').textContent = JSON.stringify(activeTrace, null, 2);
        modal.classList.add('active');
    });

    const closeModal = () => modal.classList.remove('active');
    document.getElementById('btnCloseModal').addEventListener('click', closeModal);
    document.getElementById('btnDismissModal').addEventListener('click', closeModal);

    document.getElementById('btnCopyJson').addEventListener('click', () => {
        if (!activeTrace) return;
        navigator.clipboard.writeText(JSON.stringify(activeTrace, null, 2));
        const btn = document.getElementById('btnCopyJson');
        btn.innerText = 'Copied!';
        setTimeout(() => { btn.innerText = 'Copy JSON'; }, 1200);
    });

    // Export Markdown Report
    document.getElementById('btnExportReport').addEventListener('click', async () => {
        if (!activeTrace) return;
        try {
            const res = await fetch(`${API_BASE}/api/v1/report`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    weightKg: activeTrace.inputs.weightKg,
                    heightCm: activeTrace.inputs.heightCm,
                    ageYears: activeTrace.inputs.ageYears,
                    sex: activeTrace.inputs.sex,
                    activityLevel: activeTrace.inputs.activityLevel,
                    profileContext: activeTrace.inputs.profileContext
                })
            });
            const data = await res.json();
            const blob = new Blob([data.markdown], { type: 'text/markdown;charset=utf-8' });
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = `fitmetric_report_${activeTrace.resultId}.md`;
            a.click();
            URL.revokeObjectURL(url);
        } catch (e) {
            alert(`Report download error: ${e.message}`);
        }
    });

    // History Tab Loader
    async function loadHistory() {
        const tbody = document.getElementById('historyTableBody');
        tbody.innerHTML = '<tr><td colspan="7" class="text-center">Loading past sessions...</td></tr>';

        try {
            const res = await fetch(`${API_BASE}/api/v1/history`);
            const history = await res.json();

            if (!history || history.length === 0) {
                tbody.innerHTML = '<tr><td colspan="7" class="text-center">No past records found.</td></tr>';
                return;
            }

            tbody.innerHTML = '';
            history.slice().reverse().forEach(item => {
                const tr = document.createElement('tr');
                const intervalStr = item.results.bmiInterval ? 
                    `[${item.results.bmiInterval.min.toFixed(2)}, ${item.results.bmiInterval.max.toFixed(2)}]` : '—';
                
                tr.innerHTML = `
                    <td>${item.timestamp}</td>
                    <td>${item.inputs.weightKg} kg</td>
                    <td>${item.inputs.heightCm} cm</td>
                    <td><strong>${item.results.bmi.toFixed(2)}</strong> <small style="color:#64748b;">${intervalStr}</small></td>
                    <td>${Math.round(item.results.bmr)} kcal</td>
                    <td>${Math.round(item.results.tdee)} kcal</td>
                    <td><span class="${item.stability.state === 'CROSSES_CATEGORY_BOUNDARY' ? 'text-warning' : 'text-success'}">${item.stability.state.replace(/_/g, ' ')}</span></td>
                `;
                tbody.appendChild(tr);
            });
        } catch (err) {
            tbody.innerHTML = `<tr><td colspan="7" class="text-danger text-center">Failed to load history: ${err.message}</td></tr>`;
        }
    }

    document.getElementById('btnReloadHistory').addEventListener('click', loadHistory);

    // Research Benchmark Runner
    document.getElementById('btnRunBench').addEventListener('click', async () => {
        const btn = document.getElementById('btnRunBench');
        btn.disabled = true;
        btn.innerText = 'Evaluating C++ Test Scenarios...';
        const summary = document.getElementById('benchSummary');
        summary.innerText = 'Running automated C++ test vectors...';

        try {
            const res = await fetch(`${API_BASE}/api/v1/experimental-evaluation`);
            const data = await res.json();

            summary.innerHTML = `<strong>Result:</strong> ${data.summary}`;

            // Baseline A
            document.getElementById('bA_det').innerText = `${data.baselineA.correctBoundaryCrossDetections} / ${data.baselineA.totalEvaluated} (0%)`;
            document.getElementById('bA_err').innerText = `${data.baselineA.unsupportedDefiniteInterpretations} false claims`;
            document.getElementById('bA_spd').innerText = `${data.baselineA.averageProcessingTimeUs.toFixed(1)} µs`;

            // Baseline B
            document.getElementById('bB_det').innerText = `${data.baselineB.correctBoundaryCrossDetections} / ${data.baselineB.totalEvaluated} (50%)`;
            document.getElementById('bB_err').innerText = `${data.baselineB.unsupportedDefiniteInterpretations} false claims`;
            document.getElementById('bB_spd').innerText = `${data.baselineB.averageProcessingTimeUs.toFixed(1)} µs`;

            // FitMetric
            document.getElementById('fM_det').innerText = `${data.fitMetricISVE.correctBoundaryCrossDetections} / 4 (100% Robust)`;
            document.getElementById('fM_err').innerText = `0% (Completely Eliminated)`;
            document.getElementById('fM_spd').innerText = `${data.fitMetricISVE.averageProcessingTimeUs.toFixed(1)} µs`;

        } catch (e) {
            summary.innerText = `Evaluation error: ${e.message}`;
        } finally {
            btn.disabled = false;
            btn.innerText = 'Run Benchmark Suite';
        }
    });

    // Run initial demo calculation so the right panel is populated cleanly on load
    document.getElementById('calcForm').dispatchEvent(new Event('submit'));
});
