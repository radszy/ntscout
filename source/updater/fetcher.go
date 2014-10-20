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
	"strings"
)

var cookieJar, _ = cookiejar.New(nil)

var client = &http.Client{
	Jar: cookieJar,
}

type Countries struct {
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

func login(username, password *string) bool {
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

func countries() *Countries {
	url := "http://bbapi.buzzerbeater.com/countries.aspx"

	res, err := client.Get(url)
	if err != nil {
		logger.Println(err)
		return nil
	}
	defer res.Body.Close()

	data, err := ioutil.ReadAll(res.Body)
	countries := new(Countries)
	err = xml.Unmarshal(data, countries)
	if err != nil {
		logger.Println(err)
		return nil
	}

	return countries
}

func namesEn() []string {
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

func releases() string {
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

	return releases[0].Assets[0].Browser_download_url
}

func download(url, dir string) bool {
	res, err := client.Get(url)
	if err != nil {
		logger.Println(err)
		return false
	}
	defer res.Body.Close()

	out, err := os.Create(dir + "/NTScout.zip")
	if err != nil {
		logger.Println(err)
		return false
	}
	defer out.Close()

	_, err = io.Copy(out, res.Body)
	if err != nil {
		logger.Println(err)
		return false
	}

	return true
}
