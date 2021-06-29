const char html_prefix[] PROGMEM = R"=====(
<!doctype html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">

    <title>Automated car charger</title>
  </head>
  <body>
    <nav class="navbar navbar-expand-lg navbar-light bg-light">
        <div class="container-fluid">
            <a class="navbar-brand" href="/">Automated car charger</a>
            <button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarSupportedContent" aria-controls="navbarSupportedContent" aria-expanded="false" aria-label="Toggle navigation">
            <span class="navbar-toggler-icon"></span>
            </button>
            <div class="collapse navbar-collapse" id="navbarSupportedContent">
            <ul class="navbar-nav me-auto mb-2 mb-lg-0">
                <li class="nav-item">
                <a class="nav-link active" aria-current="page" href="/config">Configuration</a>
                </li>
            </ul>
            </div>
        </div>
    </nav>

    <div class="container mt-3">
       
)=====";

const char html_home_prefix[] PROGMEM = R"=====(
        <ul class="list-group">
)=====";


const char html_home_body[] PROGMEM = R"=====(
</ul>

          <div class="card mt-3">
            <div class="card-body">
                <h5 class="card-title">Manual charging</h5>
                <a class="btn btn-primary" href="/manualCharging?duration=0" role="button">STop</a>
                <a class="btn btn-primary mx-1" href="/manualCharging?duration=1" role="button">1 hour</a>
                <a class="btn btn-primary mx-1" href="/manualCharging?duration=2" role="button">2 hour</a>
                <a class="btn btn-primary mx-1" href="/manualCharging?duration=3" role="button">3 hour</a>
                <a class="btn btn-primary mx-1" href="/manualCharging?duration=4" role="button">4 hour</a>
                <a class="btn btn-primary mx-1" href="/manualCharging?duration=5" role="button">5 hour</a>
      
            </div>
          </div>

)=====";const char html_body[] PROGMEM = R"=====(
      </div>

    
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.16.0/umd/popper.min.js"></script>
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script>
  </body>
</html>
)=====";
