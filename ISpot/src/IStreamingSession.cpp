#include "Sourcey/Spot/IStreamingSession.h"
#include "Sourcey/Spot/IStreamingManager.h"
#include "Sourcey/Spot/IChannel.h"
#include "Sourcey/CryptoProvider.h"


using namespace std;
using namespace Poco;


namespace Sourcey {
namespace Spot {

	
// ---------------------------------------------------------------------
//
StreamingParams::StreamingParams(
	const string& peer,			// The initiating peer ID
	const string& channel,		// The streaming channel name
	const string& transport,	// UDP, TCP, TLS
	const string& protocol,		// Raw, HTTP, RTP/AVP
	const string& encoding,		// None, Base64, ...
	const Media::Format& iformat,
	const Media::Format& oformat,		
	int timeout) :
		Media::EncoderParams(iformat, oformat),
		token(CryptoProvider::generateRandomKey(32)),
		peer(peer),
		channel(channel),
		transport(transport),
		protocol(protocol),
		encoding(encoding),
		timeout(timeout) 
{
}		


void StreamingParams::serialize(JSON::Value& root)
{
	root["token"] = token;
	root["peer"] = peer;
	root["timeout"] = timeout;
	root["transport"] = transport;
	root["protocol"] = protocol;
	root["encoding"] = encoding;
	root["format"] = oformat.label;

	// BUG: JSON Value does not seem to support
	// switching references after assignment.
	JSON::Value& v = root["video"];
	if (oformat.video.enabled) {
		v["codec"] = Media::Codec::idString(oformat.video.id);
		v["width"] = Util::itoa(oformat.video.width);
		v["height"] = Util::itoa(oformat.video.height);
		v["fps"] = Util::itoa(oformat.video.fps);
	}
	else v = Json::nullValue;
	
	JSON::Value& a = root["audio"];
	if (oformat.audio.enabled) {
		a["codec"] = Media::Codec::idString(oformat.audio.id);
		a["bit-rate"] = Util::itoa(oformat.audio.bitRate);
		a["channels"] = Util::itoa(oformat.audio.channels);
		a["sample-rate"] = Util::itoa(oformat.audio.sampleRate);
	}
	else a = Json::nullValue;
}


void StreamingParams::deserialize(JSON::Value& root)
{	
	if (root.isMember("token")) token = root["token"].asString();
	if (root.isMember("timeout") && root["timeout"].isInt()) 
		timeout = root["timeout"].asInt();

	// Transport
	transport = root.isMember("transport") ? root["transport"].asString() : "TCP";
	if (transport != "TCP" &&
		transport != "TSL" &&
		transport != "UDP")
		throw Exception("Unsupported transport protocol.");

	protocol = root.isMember("protocol") ? root["protocol"].asString() : "Raw";	
	encoding = root.isMember("encoding") ? root["encoding"].asString() : "None";

	// Video
	JSON::Value& v = root["video"];
	//Log("debug") << "[StreamingParams] Updating: " << JSON::stringify(root, true) << endl;
	//Log("debug") << "[StreamingParams] Updating: " << JSON::stringify(v, true) << endl;
	if (!v.isNull()) {		
		oformat.video.enabled = v.isMember("enabled") ? v["enabled"].asBool() : true;
		if (oformat.video.enabled) {
			if (v.isMember("codec")) oformat.video.id = Media::Codec::toID(v["codec"].asString());
			if (v.isMember("width")) oformat.video.width = Util::atoi(v["width"].asString());	
			if (v.isMember("height")) oformat.video.height = Util::atoi(v["height"].asString());
			if (v.isMember("fps")) oformat.video.fps = Util::atoi(v["fps"].asString());
		}
	}
	
	// Audio
	JSON::Value& a = root["audio"];
	//oformat.audio.enabled = !a.isNull() && a["enabled"].asBool();
	if (oformat.audio.enabled) {
		if (a.isMember("codec")) oformat.audio.id = Media::Codec::toID(a["codec"].asString());
		if (a.isMember("bit-rate")) oformat.audio.bitRate = Util::atoi(a["bit-rate"].asString());
		if (a.isMember("channels")) oformat.audio.channels = Util::atoi(a["channels"].asString());
		if (a.isMember("sample-rate")) oformat.audio.sampleRate = Util::atoi(a["sample-rate"].asString());
	}
}

	
bool StreamingParams::valid()
{
	return !peer.empty()
		&& !channel.empty()
		&& !token.empty()
		&& (transport == "TCP" 
		||  transport == "TSL" 
		||  transport == "UDP") 
		&& (oformat.video.enabled 
		||  oformat.audio.enabled);
}


// ---------------------------------------------------------------------
//
IStreamingSession::IStreamingSession(IEnvironment& env, 
									 IStreamingManager& service, 
									 const StreamingParams& params) :
	IModule(env),
	_service(service),
	_params(params)
{		
	assert(_params.valid());

	log() << "Creating:"		
		<< "\n\tPID: " << this	
		<< "\n\tChannel: " << _params.channel
		<< "\n\tFormat: " << _params.oformat.label
		<< "\n\tUsing Video: " << _params.oformat.video.enabled
		<< "\n\tUsing Audio: " << _params.oformat.audio.enabled
		<< "\n\tPeer: " << _params.peer
		<< "\n\tTransport: " << _params.transport
		<< "\n\tEncoding: " << _params.protocol
		<< "\n\tToken: " << _params.token
		<< endl;
}

	
IStreamingSession::~IStreamingSession()
{
	log() << "Destroying"<< endl;
	assert(isTerminating());
}


void IStreamingSession::initialize()
{
	//_stream.StateChange += delegate(this, &StreamingSession::onSessionStreamStateChange);
	// The stream will be initialized on connection.
	// TODO: The current design model doesn't allow
	// for encoder errors. Perhaps we need a way to 
	// test the encoder stream?

	setState(StreamingState::Initializing);
}


void IStreamingSession::terminate()
{
	log() << "Terminating" << endl;
	assert(!isTerminating());

	// Set the state here as we may have stream
	// adapters listening for this signal.
	setState(StreamingState::Terminating);
}


void IStreamingSession::notifyCandidates()
{
	assert(isActive());
	CandidateList c = candidates();
	CandidatesCollected.dispatch(this, c);
}


void IStreamingSession::setError(const string& reason)
{
	setState(StreamingState::Error, reason);
}


bool IStreamingSession::setState(unsigned int id, const string& message) 
{
	// Overriding this method so the state signal
	// will always be sent from the session base.
	return StatefulSignal<StreamingState>::setState(this, id, message);
}


void IStreamingSession::addCandidate(const string& type, 
									 const Net::Address& address, 
									 const string& uri)
{
	FastMutex::ScopedLock lock(_mutex);
	_candidates.push_back(Candidate(type, address, uri));
}


bool IStreamingSession::removeCandidate(const string& type, 
										const Net::Address& address)
{
	bool res = false;
	FastMutex::ScopedLock lock(_mutex);
	for (CandidateList::iterator it = _candidates.begin(); it != _candidates.end(); ++it) {	
		if ((*it).type == type && (*it).address == address) {
			_candidates.erase(it);
			res = true;
			break;
		}
	}

	return res;
}


bool IStreamingSession::isActive() const			
{ 
	return stateEquals(StreamingState::Ready)
		|| stateEquals(StreamingState::Active); 
}


bool IStreamingSession::isError() const
{
	return stateEquals(StreamingState::Error); 
}


bool IStreamingSession::isTerminating() const		
{ 
	return stateEquals(StreamingState::Terminating); 
}


IStreamingManager& IStreamingSession::service()		
{ 
	FastMutex::ScopedLock lock(_mutex);
	return _service; 
}


ConnectionStreamList IStreamingSession::connections() const
{ 
	FastMutex::ScopedLock lock(_mutex);
	return _connections; 
}


string IStreamingSession::token() const		
{ 
	FastMutex::ScopedLock lock(_mutex);
	return _params.token; 
}


PacketStream& IStreamingSession::stream()		
{ 
	FastMutex::ScopedLock lock(_mutex);
	return _stream; 
}


CandidateList IStreamingSession::candidates() const
{ 
	FastMutex::ScopedLock lock(_mutex);
	return _candidates; 
}


StreamingParams& IStreamingSession::params()	
{ 
	FastMutex::ScopedLock lock(_mutex);
	return _params; 
}

	
} } // namespace Sourcey::Spot



/*, 
									 IChannel& channel, 
									 Symple::Peer& peer, 
									 const Symple::Command& command*/
	//_channel(channel),
	//,
	//_peer(peer),
	//_command(command)
	/*
	// Dereference all connections and erase callbacks.
	ConnectionStreamList::iterator it = _connections.begin();
	while (it != _connections.end()) {
		log() << "Dereferenced Connection: " << *it << endl;
		(*it)->StateChange -= delegate(this, &IStreamingSession::onConnectionStreamStateChange);
		(*it)->close();
		it = _connections.erase(it);
	}

	// Close the packet stream. This will cause
	// the  destruction of all managed adapters.
	_stream.StateChange -= delegate(this, &IStreamingSession::onSessionStreamStateChange);
	_stream.close();
	*/

/*
Symple::Peer& IStreamingSession::peer()
{
	FastMutex::ScopedLock lock(_mutex);
	return _peer;
}


IChannel& IStreamingSession::channel()		
{ 
	FastMutex::ScopedLock lock(_mutex);
	return _channel; 
}


Symple::Command& IStreamingSession::command()		
{ 
	FastMutex::ScopedLock lock(_mutex);
	return _command; 
}
*/

	/*
	// TODO: Remove matches to avoid doubling up.
	JSON::Value cand;
	cand["type"] = type;
	cand["address"] = address.toString();
	cand["uri"] = uri;
	_command.data("candidates").append(cand);
	*/
	/*
	JSON::Value& cands = _command.data("candidates");
	if (cands.size()) {
		JSON::Value copy(cands); // copy items
		cands.clear();
		for (JSON::ValueIterator it = copy.begin(); it != copy.end(); it++) {		
			JSON::Value& cand = *it;
			if (cand["type"].asString() != type &&
				cand["address"].asString() != address.toString())
				cands.append(cand);
			else res = true;
		}
	}
	*/

/*
ConnectionStream* IStreamingSession::createConnection()
{
	log() << "Creating Connection" << endl;
	
	FastMutex::ScopedLock lock(_mutex);

	ConnectionStream* conn = new ConnectionStream();
	conn->StateChange += delegate(this, &IStreamingSession::onConnectionStreamStateChange);	
	conn->attach(&_stream, false, false);
	// NOTE: stream not started here 

	_connections.push_back(conn);
	if (_connections.size() == 1) {		
		_stream.start(); // start the packet stream & sources
		setState(StreamingState::Active);
	}

	return conn;
}


TCPStreamingConnection* IStreamingSession::createTCPConnection(const Poco::Net::StreamSocket& socket,
													     const string& request)
{		
	log() << "Creating TCP Connection: " << request << endl;

	ConnectionStream* conn = createConnection();
	return new TCPStreamingConnection(conn, socket, true, false);
}
*/

/*



void IStreamingSession::onConnectionClosing(void* sender, int)
{
	FastMutex::ScopedLock lock(_mutex);
}
*/

/*
string IStreamingSession::token() const		
{ 
	return _command.data("token"];
}




bool IStreamingSession::setState(unsigned int id)
{
	assert(state().id() != id);

	if (!Stateful<StreamingState>::setState(id))
		return false;	
	

	log() << "State Changed: " << state() << endl;

	switch (state().id()) {
	case StreamingState::Waiting:
		// Waiting media sessions will have [timeout]
		// duration to reconnect before the session expires.
		startTimer();
		//stop();
		break;

	case StreamingState::Active:
		// Reset and start the internal packet stream 
		// adapters and stop the timers.
		//reset();
		//start();
		//stopTimer();
		break;

	//case StreamingState::Error:
		//startTimer();
		//stop();
		//break;

	case StreamingState::Terminating:
		//stopTimer();
		//stop();
		break;
	}

	return true;
}
*/


/*
void IStreamingSession::startTimer(int timeout) 
{
	if (timeout || _command.timeout)
		Timer::getDefault().start(TimerCallback<IStreamingSession>(this, &IStreamingSession::onTimeout, 
			timeout > 0 ? timeout : _command.timeout));
}


void IStreamingSession::stopTimer() 
{	
	if (_command.timeout)
		Timer::getDefault().stop(TimerCallback<IStreamingSession>(this, &IStreamingSession::onTimeout));
}


void IStreamingSession::onTimeout(TimerCallback<IStreamingSession>& timer) 
{
	log() << "Timed Out: " << timer.startInterval() << endl;
	setStateMessage("The media session timed out.");
	terminate();
}
*/