from flask import Flask, request, jsonify
import firebase_admin
from firebase_admin import auth, credentials

app = Flask(__name__)

# Initialize Firebase using a service account key JSON file.
# Replace 'path/to/serviceAccountKey.json' with the path to your own file.
cred = credentials.Certificate('path/to/serviceAccountKey.json')
firebase_admin.initialize_app(cred)

@app.route('/login', methods=['POST'])
def login():
    """Verify Firebase ID token sent by the client."""
    data = request.get_json(silent=True) or {}
    id_token = data.get('idToken')
    if not id_token:
        return jsonify({'error': 'Missing idToken'}), 400
    try:
        decoded = auth.verify_id_token(id_token)
        uid = decoded['uid']
        return jsonify({'uid': uid, 'message': 'Authentication successful'})
    except Exception as e:
        return jsonify({'error': str(e)}), 401

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
