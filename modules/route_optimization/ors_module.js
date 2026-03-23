const axios = require('axios');
const fs = require('fs');
const path = require('path');

// ==========================================
// 🔑 API KEY INSERTED
// ==========================================
const ORS_API_KEY = "eyJvcmciOiI1YjNjZTM1OTc4NTExMTAwMDFjZjYyNDgiLCJpZCI6ImQ2ZTUxZjk1MDIyNzQxOGI4YTljNzBmOTVjZTVjZTQ3IiwiaCI6Im11cm11cjY0In0=";

// Check if key is missing
if (!ORS_API_KEY) {
  console.warn('⚠️  WARNING: ORS API Key is missing. Logistics will fail.');
}

/**
 * Get route between two sets of coordinates.
 * Note: ORS expects [Longitude, Latitude] order in the API body.
 */
async function getRouteByCoords(startLat, startLng, endLat, endLng) {
  if (!ORS_API_KEY) return { status: 'error', message: 'ORS_API_KEY not configured' };
  
  // Validate inputs
  if ([startLat, startLng, endLat, endLng].some(v => v === undefined || v === null || v === '')) {
    return { status: 'error', message: 'Missing coordinates' };
  }

  const url = 'https://api.openrouteservice.org/v2/directions/driving-car/geojson';
  
  const body = {
    coordinates: [
      [Number(startLng), Number(startLat)], // Start [Long, Lat]
      [Number(endLng), Number(endLat)]      // End [Long, Lat]
    ]
  };

  try {
    const res = await axios.post(url, body, {
      headers: {
        'Authorization': ORS_API_KEY,
        'Content-Type': 'application/json'
      }
    });

    if (!res.data || !res.data.features || !res.data.features.length) {
      return { status: 'error', message: 'No route found between these points' };
    }

    const route = res.data.features[0];
    const summary = route.properties.summary;
    
    return {
      status: 'success',
      // Convert meters to km
      distance_km: Number((summary.distance / 1000).toFixed(2)),
      // Convert seconds to minutes
      duration_min: Number((summary.duration / 60).toFixed(2)),
      // The LineString geometry (array of [long, lat] points) to draw on map
      geometry: route.geometry.coordinates 
    };

  } catch (err) {
    // Safe error handling
    const errorDetails = err.response ? JSON.stringify(err.response.data) : err.message;
    console.error("ORS API Error:", errorDetails);
    return { status: 'error', message: 'Route calculation failed. Check API Key validity.' };
  }
}

/**
 * Helper to lookup Lat/Lng from locations.json by Name (ID)
 */
async function getRouteByEntities(buyerId, farmerId, mappingPath = path.join(__dirname, 'locations.json')) {
  if (!fs.existsSync(mappingPath)) {
    return { status: 'error', message: `Mapping file not found: ${mappingPath}` };
  }

  let map;
  try { 
      map = JSON.parse(fs.readFileSync(mappingPath, 'utf8')); 
  } catch (e) { 
      return { status: 'error', message: 'Invalid locations.json file' }; 
  }

  // Handle different JSON structures
  const users = map.users || {};
  const buyer = users[buyerId];
  const farmer = users[farmerId];

  if (!buyer) return { status: 'error', message: `Location not found for user: '${buyerId}'` };
  if (!farmer) return { status: 'error', message: `Location not found for user: '${farmerId}'` };

  // Call the coordinate function
  return getRouteByCoords(buyer.lat, buyer.lng, farmer.lat, farmer.lng);
}

module.exports = { getRouteByCoords, getRouteByEntities };