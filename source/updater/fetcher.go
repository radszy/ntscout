package main

import (
	"bytes"
	"encoding/json"
	"encoding/xml"
	"io"
	"io/ioutil"
	"net/http"
	"net/http/cookiejar"
	"os"
	"strconv"
	"strings"
)

var cookieJar, _ = cookiejar.New(nil)

var client = &http.Client{
	Jar: cookieJar,
}

type CountryData struct {
	XMLName xml.Name  `xml:"bbapi"`
	Country []Country `xml:"countries>country"`
}

type Country struct {
	XMLName   xml.Name `xml:"country"`
	ID        string   `xml:"id,attr"`
	Divisions string   `xml:"divisions,attr"`
	Users     string   `xml:"users,attr"`
	Name      string   `xml:",chardata"`
	NameEn    string
}

func Login(username, password *string) bool {
	url := "http://bbapi.buzzerbeater.com/login.aspx?" +
		"login=" + *username + "&code=" + *password

	res, err := client.Get(url)
	if err != nil {
		logger.Println(err)
		return false
	}
	defer res.Body.Close()

	data, err := ioutil.ReadAll(res.Body)
	if err != nil {
		logger.Println(err)
		return false
	}

	return bytes.Contains(data, []byte("loggedIn"))
}

func Countries() *CountryData {
	url := "http://bbapi.buzzerbeater.com/countries.aspx"

	res, err := client.Get(url)
	if err != nil {
		logger.Println(err)
		return nil
	}
	defer res.Body.Close()

	data, err := ioutil.ReadAll(res.Body)
	countries := new(CountryData)
	err = xml.Unmarshal(data, countries)
	if err != nil {
		logger.Println(err)
		return nil
	}

	return countries
}

func TranslatedNames() []string {
	url := "https://raw.githubusercontent.com/" +
		"rsxee/NTScout/master/names-en.txt"

	res, err := client.Get(url)
	if err != nil {
		return nil
	}
	defer res.Body.Close()

	data, err := ioutil.ReadAll(res.Body)
	if err != nil {
		logger.Println(err)
		return nil
	}

	return strings.Split(string(data), "\n")
}

func Releases(ver float64) string {
	url := "https://api.github.com/repos/rsxee/NTScout/releases"
	res, err := client.Get(url)
	if err != nil {
		logger.Println(err)
		return ""
	}
	defer res.Body.Close()
	data, err := ioutil.ReadAll(res.Body)
	if err != nil {
		logger.Println(err)
		return ""
	}

	type Releases struct {
		Url      string
		Tag_name string
		Assets   []struct {
			Browser_download_url string
		}
	}
	var releases []Releases
	err = json.Unmarshal(data, &releases)
	if err != nil {
		logger.Println(err)
		return ""
	}

	if len(releases) == 0 {
		logger.Println("no releases found")
		return ""
	}

	tag, err := strconv.ParseFloat(releases[0].Tag_name, 64)
	if tag <= ver {
		logger.Println("no new releases")
		return ""
	}

	return releases[0].Assets[0].Browser_download_url
}

func DownloadRelease(url, dir string) bool {
	res, err := client.Get(url)
	defer res.Body.Close()
	if err != nil {
		logger.Println(err)
		return false
	}

	out, err := os.Create(dir + "/NTScout.zip")
	defer out.Close()
	if err != nil {
		logger.Println(err)
		return false
	}

	_, err = io.Copy(out, res.Body)
	if err != nil {
		logger.Println(err)
		return false
	}

	return true
}

func DownloadFlags(id, dir string) bool {
	url := "http://www.buzzerbeater.com/images/flags/flag_" + id + ".gif"
	res, err := client.Get(url)
	defer res.Body.Close()
	if err != nil {
		logger.Println(err)
		return false
	}

	if res.StatusCode != 404 {
		out, err := os.Create(dir + "/flags/flag_" + id + ".gif")
		defer out.Close()
		if err != nil {
			logger.Println(err)
			return false
		}

		_, err = io.Copy(out, res.Body)
		if err != nil {
			logger.Println(err)
			return false
		}
	}

	return true
}
