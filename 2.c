using System;
using System.Net.Http;
using System.Text.Json;
using System.Threading.Tasks;

public class IMDSv2CredentialService
{
    private readonly HttpClient _httpClient;
    private readonly IMDSv2TokenService _tokenService;
    private const string IAM_ROLE_URL = "http://169.254.169.254/latest/meta-data/iam/security-credentials/";

    public IMDSv2CredentialService(HttpClient httpClient, IMDSv2TokenService tokenService)
    {
        _httpClient = httpClient;
        _tokenService = tokenService;
    }

    public async Task<string> GetIAMCredentialsAsync()
    {
        var token = await _tokenService.GetTokenAsync();
        var request = new HttpRequestMessage(HttpMethod.Get, IAM_ROLE_URL);
        request.Headers.Add("X-aws-ec2-metadata-token", token);

        var response = await _httpClient.SendAsync(request);
        response.EnsureSuccessStatusCode();

        var roleName = await response.Content.ReadAsStringAsync();

        // Agora pegamos as credenciais da role encontrada
        var roleCredentialsUrl = $"{IAM_ROLE_URL}{roleName}";
        var credentialsRequest = new HttpRequestMessage(HttpMethod.Get, roleCredentialsUrl);
        credentialsRequest.Headers.Add("X-aws-ec2-metadata-token", token);

        var credentialsResponse = await _httpClient.SendAsync(credentialsRequest);
        credentialsResponse.EnsureSuccessStatusCode();

        var credentialsJson = await credentialsResponse.Content.ReadAsStringAsync();
        return credentialsJson;
    }
}
