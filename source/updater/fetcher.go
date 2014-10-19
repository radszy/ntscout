package main

import (
	"bytes"
	"encoding/json"
	"encoding/xml"
	"errors"
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

func login(username, password *string) (bool, error) {
	url := "http://bbapi.buzzerbeater.com/login.aspx?" +
		"login=" + *username + "&code=" + *password

	res, err := client.Get(url)
	if err != nil {
		return false, err
	}
	defer res.Body.Close()

	data, err := ioutil.ReadAll(res.Body)

	return bytes.Contains(data, []byte("loggedIn")), nil
}

func countries() (*Countries, error) {
	url := "http://bbapi.buzzerbeater.com/countries.aspx"

	res, err := client.Get(url)
	if err != nil {
		return nil, err
	}
	defer res.Body.Close()

	data, err := ioutil.ReadAll(res.Body)
	countries := new(Countries)
	err = xml.Unmarshal(data, countries)
	if err != nil {
		return nil, err
	}

	return countries, nil
}

func namesEn() ([]string, error) {
	url := "https://raw.githubusercontent.com/" +
		"rsxee/NTScout/master/names-en.txt"

	res, err := client.Get(url)
	if err != nil {
		return nil, nil
	}
	defer res.Body.Close()

	data, err := ioutil.ReadAll(res.Body)
	names := strings.Split(string(data), "\n")

	return names, nil
}

func releases() (string, error) {
	url := "https://api.github.com/repos/rsxee/NTScout/releases"
	res, err := client.Get(url)
	if err != nil {
		return "", err
	}
	defer res.Body.Close()
	data, err := ioutil.ReadAll(res.Body)

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
		return "", err
	}

	if len(releases) == 0 {
		return "", errors.New("No releases found.")
	}

	return releases[0].Assets[0].Browser_download_url, nil
}

func download(url, dir string) error {
	res, err := client.Get(url)
	if err != nil {
		return err
	}
	defer res.Body.Close()

	out, err := os.Create(dir + "/NTScout.zip")
	if err != nil {
		return err
	}
	defer out.Close()

	_, err = io.Copy(out, res.Body)
	if err != nil {
		return err
	}

	return nil
}
