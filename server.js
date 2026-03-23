/* --- GreenMart Web Server (Node.js) --- */
const express = require('express');
const { execFile } = require('child_process');
const path = require('path');

// --- 1. Import Custom Modules ---
// ORS Module for Real-World Routing
let ors;
try {
    ors = require('./modules/route_optimization/ors_module');
} catch (e) {
    console.warn("Warning: ORS Module not found. Logistics will fail.", e);
}

const app = express();
const port = 3000;

// --- 2. Configuration ---
const EXEC_NAME = process.platform === 'win32' ? 'greenmart_engine.exe' : './greenmart_engine';
const C_ENGINE_PATH = path.join(__dirname, EXEC_NAME);

app.use(express.static('public'));
app.use(express.json());

// --- 3. Helper: Run C Engine ---
function runCEngine(args) {
    return new Promise((resolve, reject) => {
        execFile(C_ENGINE_PATH, args, (error, stdout, stderr) => {
            if (error) {
                console.error(`[C Engine Error] Cmd: ${args.join(' ')}`);
                try { resolve(JSON.parse(stdout)); } 
                catch (e) { reject({ status: "error", message: "Engine Execution Failed." }); }
            } else {
                try {
                    const json = JSON.parse(stdout.trim());
                    resolve(json);
                } catch (e) {
                    console.error(`[JSON Parse Error] Output: ${stdout}`);
                    reject({ status: "error", message: "Engine output was not valid JSON." });
                }
            }
        });
    });
}

// =================================================
// 4. API ENDPOINTS
// =================================================

app.post('/api/login', async (req, res) => {
    try {
        const result = await runCEngine(['login', req.body.username, req.body.password]);
        res.json(result);
    } catch (e) { res.status(500).json(e); }
});

app.post('/api/register', async (req, res) => {
    try {
        const result = await runCEngine(['register', req.body.userid, req.body.name, req.body.contact, req.body.type, req.body.password]);
        res.json(result);
    } catch (e) { res.status(500).json(e); }
});

app.post('/api/addProduct', async (req, res) => {
    try {
        const { name, qty, price, expiry_date, farmer_id } = req.body;
        const expiry = new Date(expiry_date);
        const now = new Date();
        const diffDays = Math.ceil((expiry - now) / (1000 * 60 * 60 * 24));
        const args = ['addProduct', name, String(qty), String(price), String(diffDays), farmer_id];
        const result = await runCEngine(args);
        res.json(result);
    } catch (e) { res.status(500).json(e); }
});

app.post('/api/getInventory', async (req, res) => {
    try {
        const result = await runCEngine(['getInsights']);
        if(result.status === 'success') res.json({ status: 'success', inventory: result.data.heap });
        else res.json(result);
    } catch (e) { res.status(500).json(e); }
});

app.post('/api/getHistory', async (req, res) => {
    try {
        const result = await runCEngine(['getInsights']);
        if(result.status === 'success') res.json({ status: 'success', history: result.data.requests });
        else res.json(result);
    } catch (e) { res.status(500).json(e); }
});

app.post('/api/submitOrder', async (req, res) => {
    const { buyer_id, cart_json } = req.body; 
    if(!cart_json) return res.json({status:'error', message: 'Cart is empty'});
    const items = cart_json.split(';');
    let billLog = [];
    let totalCost = 0;
    try {
        for (let itemStr of items) {
            if(!itemStr) continue;
            const [product, qty] = itemStr.split(',');
            // Instant Allocation Call
            const result = await runCEngine(['addRequest', buyer_id, product, qty]);
            
            if(result.status === 'success' && result.bill) {
                totalCost += result.bill.total_cost;
                billLog.push({ 
                    product: product, 
                    status: 'FULFILLED', 
                    message: `Bought from Farmer ${result.bill.farmer_id} (Rs. ${result.bill.total_cost.toFixed(2)})` 
                });
            } else {
                billLog.push({ 
                    product: product, 
                    status: 'PENDING', 
                    message: result.message || "Out of Stock" 
                });
            }
        }
        res.json({ status: 'success', total_cost: totalCost, bill: billLog });
    } catch (e) { res.status(500).json({ status: 'error', message: 'Server Error processing order.' }); }
});

app.post('/api/cancelRequest', async (req, res) => {
    try {
        const result = await runCEngine(['cancelRequest', req.body.req_id]);
        res.json(result);
    } catch (e) { res.status(500).json(e); }
});

// --- NEW: Real-World Route Optimization (ORS) ---
app.post('/api/optimizeRouteByEntities', async (req, res) => {
    try {
        const { buyer_id, farmer_id } = req.body;
        if (!buyer_id || !farmer_id) return res.json({ status: 'error', message: 'Missing buyer_id or farmer_id' });
        
        // Call the ORS module
        const result = await ors.getRouteByEntities(buyer_id, farmer_id);
        return res.json(result);
    } catch (e) {
        console.error("Route Error:", e);
        return res.status(500).json({ status: 'error', message: 'Routing failed internal error' });
    }
});

app.listen(port, () => {
    console.log(`\n>>> GreenMart Server running at http://localhost:${port}`);
});